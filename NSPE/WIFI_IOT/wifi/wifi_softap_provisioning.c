/*!
    \file    wifi_softap_provisioning.c
    \brief   provsioning for WiFi softap mode

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "app_cfg.h"
#include "wrapper_os.h"
#include "lwip/sockets.h"
#include "lwip/apps/httpd.h"
#include "lwip/tcpip.h"
#include "lwip/err.h"
#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "wifi_softap_provisioning.h"
#include "dnsd.h"

#ifdef CONFIG_SOFTAP_PROVISIONING

static os_task_t* provisioning_task_tcb = NULL;

static uint8_t config_ssid[WIFI_SSID_MAX_LEN + 1];
static uint8_t config_password[WPA_MAX_PSK_LEN + 1];

const uint8_t ap_ssid[] = "wifi_provisioning";
const uint8_t ap_password[] = "12345678";

#define PROVISIONING_TASK_STK_SIZE                  512
#define PROVISIONING_TASK_QUEUE_SIZE                4
#define PROVISIONING_TASK_PRIO                      16

typedef enum {
    PROVISIONING_MSG_CONFIGURED = 1,
    PROVISIONING_MSG_STA_CONNECT_OK,
    PROVISIONING_MSG_STA_CONNECT_FAILED,
    PROVISIONING_MSG_STOP,
}SOFTAP_PROVISIONING_MESSAGE_TYPE_E;

typedef enum {
    PROVISIONING_STATE_IDLE = 0,
    PROVISIONING_STATE_WAIT_CONFIGURED,
    PROVISIONING_STATE_STA_CONNECTING,
    PROVISIONING_STATE_SUCCESSFUL,
}SOFTAP_PROVISIONING_STATE_E;

static void wifi_softap_provisonging_send_msg(SOFTAP_PROVISIONING_MESSAGE_TYPE_E msg_type)
{

    SOFTAP_PROVISIONING_MESSAGE_TYPE_E msg = msg_type;

    if (provisioning_task_tcb == NULL)
        return;

    sys_task_post(provisioning_task_tcb, &msg, 0);
}


static void sta_cb_conn_ok(void *eloop_data, void *user_ctx)
{
    eloop_event_unregister(WIFI_MGMT_EVENT_CONNECT_SUCCESS);
    eloop_event_unregister(WIFI_MGMT_EVENT_CONNECT_FAIL);
    wifi_softap_provisonging_send_msg(PROVISIONING_MSG_STA_CONNECT_OK);
}


static void sta_cb_conn_fail(void *eloop_data, void *user_ctx)
{
    eloop_event_unregister(WIFI_MGMT_EVENT_CONNECT_SUCCESS);
    eloop_event_unregister(WIFI_MGMT_EVENT_CONNECT_FAIL);
    wifi_softap_provisonging_send_msg(PROVISIONING_MSG_STA_CONNECT_FAILED);
}

static void wifi_softap_provisioning(void *arg)
{
    SOFTAP_PROVISIONING_MESSAGE_TYPE_E msg_type;
    SOFTAP_PROVISIONING_STATE_E state = PROVISIONING_STATE_IDLE;
    httpd_init();
    dns_server_start();

    wifi_management_ap_start((char *)ap_ssid, (char *)ap_password, 1, 0);
    state = PROVISIONING_STATE_WAIT_CONFIGURED;

    while (1) {
        if (sys_task_wait(0, &msg_type) == OS_OK) {
            if (msg_type == PROVISIONING_MSG_CONFIGURED) {
                if (state == PROVISIONING_STATE_WAIT_CONFIGURED) {
                    DEBUGPRINT("softap provisioning got configure, start connecting\r\n");
                    state = PROVISIONING_STATE_STA_CONNECTING;
                    wifi_management_sta_start();

                    if (wifi_management_connect(config_ssid, config_password, 0) < 0) {
                        DEBUGPRINT("softap provisioning start connecting failed\r\n");
                        wifi_management_ap_start((char *)ap_ssid, (char *)ap_password, 1, 0);
                        state = PROVISIONING_STATE_WAIT_CONFIGURED;
                    } else {
                        eloop_event_register(WIFI_MGMT_EVENT_CONNECT_SUCCESS, sta_cb_conn_ok, NULL, NULL);
                        eloop_event_register(WIFI_MGMT_EVENT_CONNECT_FAIL, sta_cb_conn_fail, NULL, NULL);
                    }
                } else {
                    DEBUGPRINT("softap provisioning got dulicate configure, state %d\r\n", state);
                }
            } else if (msg_type == PROVISIONING_MSG_STA_CONNECT_OK) {
                state = PROVISIONING_STATE_SUCCESSFUL;
                DEBUGPRINT("softap provisioning connnect ok, exit provision\r\n");
                break;
            }
            else if (msg_type == PROVISIONING_MSG_STA_CONNECT_FAILED) {
                DEBUGPRINT("softap provisioning connnect failed, restart softap\r\n");
                wifi_management_ap_start((char *)ap_ssid, (char *)ap_password, 1, 0);
                state = PROVISIONING_STATE_WAIT_CONFIGURED;
            }
            else if (msg_type == PROVISIONING_MSG_STOP) {
                DEBUGPRINT("softap provisioning stop\r\n");
                state = PROVISIONING_STATE_IDLE;
                break;
            }
        }
    }

    // Other task should lock tcpip core if call raw api
    LOCK_TCPIP_CORE();
    httpd_stop();
    UNLOCK_TCPIP_CORE();

    dns_server_stop();
    provisioning_task_tcb = NULL;
    DEBUGPRINT("softap provisioning exit, state %d\r\n", state);
    sys_task_delete(NULL);
}

void wifi_softap_provisioning_start(void)
{
    if (provisioning_task_tcb)
        return;

    provisioning_task_tcb = sys_task_create(NULL, (const uint8_t *)"ap_prov", NULL, PROVISIONING_TASK_STK_SIZE, PROVISIONING_TASK_QUEUE_SIZE, PROVISIONING_TASK_PRIO,
                    (task_func_t)wifi_softap_provisioning, NULL);

    if (provisioning_task_tcb == NULL)
        DEBUGPRINT("softap provisioning start failed\r\n");
}

void wifi_softap_provisioning_stop(void)
{
    wifi_softap_provisonging_send_msg(PROVISIONING_MSG_STOP);
}

int32_t wifi_softap_provisioning_config(uint8_t *ssid, uint8_t *pass)
{
    uint32_t len;
    int32_t ret = 0;

    do {
        len = strlen((const char *)ssid);
        if (len > WIFI_SSID_MAX_LEN) {
            ret = -1;
            break;
        }
        sys_memcpy(config_ssid, ssid, len);
        config_ssid[len] = 0;

        len = strlen((const char *)pass);
        if (len > WPA_MAX_PSK_LEN || len < 8) {
            ret = -1;
            break;
        }
        sys_memcpy(config_password, pass, len);
        wifi_softap_provisonging_send_msg(PROVISIONING_MSG_CONFIGURED);
    } while(0);

    return ret;
}
#endif