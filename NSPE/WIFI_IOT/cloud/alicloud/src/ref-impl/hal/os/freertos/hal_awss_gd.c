#include "app_cfg.h"

#if defined(CONFIG_ALICLOUD_SUPPORT)
#include <stdio.h>

#include "iot_import.h"
#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "wrapper_os.h"

static awss_recv_80211_frame_cb_t g_ieee80211_handler = NULL;
static void monitor_data_handler(unsigned char *buf, unsigned short len, signed char rssi)
{
    int with_fcs  = 0;
    enum AWSS_LINK_TYPE link_type = AWSS_LINK_TYPE_NONE;

    if (buf == NULL) {
        return;
    }

    if (g_ieee80211_handler != NULL) {
        // platform_warn("monitor_data_handler len %d rssi %d\r\n", len, rssi);
        (*g_ieee80211_handler)((char *)buf, (int)len, link_type, with_fcs, rssi);
    }
}

void HAL_Awss_Open_Monitor(_IN_ awss_recv_80211_frame_cb_t cb)
{
    platform_warn("HAL_Awss_Open_Monitor\r\n");
    g_ieee80211_handler = cb;
    wifi_netlink_promisc_mode_set(1, monitor_data_handler);
    HAL_Awss_Switch_Channel(6, 0, NULL);
}

void HAL_Awss_Close_Monitor(void)
{
    platform_warn("HAL_Awss_Close_Monitor\r\n");
    wifi_netlink_promisc_mode_set(0, NULL);
}

int HAL_Awss_Connect_Ap(
    _IN_ uint32_t connection_timeout_ms,
    _IN_ char ssid[HAL_MAX_SSID_LEN],
    _IN_ char passwd[HAL_MAX_PASSWD_LEN],
    _IN_OPT_ enum AWSS_AUTH_TYPE auth,
    _IN_OPT_ enum AWSS_ENC_TYPE encry,
    _IN_OPT_ uint8_t bssid[ETH_ALEN],
    _IN_OPT_ uint8_t channel)
{
    int32_t ret = 0;
    uint32_t retry = 2;
    uint32_t i;

    if (ssid == NULL) {
        platform_warn("connect to AP, ssid is NULL\r\n");
        return -1;
    }
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    ret = wifi_management_connect((uint8_t *)ssid, (uint8_t *)passwd, FALSE);
    if (ret != 0) {
        platform_warn("start wifi_connect failed\r\n");
    }

    for (i = 0; i < 30; i++) {
        sys_ms_sleep(1000);
        if (wifi_netif_is_ipv4_got()
        #ifdef CONFIG_IPV6_SUPPORT
            || wifi_netif_is_ipv6_got()
        #endif
        ) {
            return 0;
        }
    }
#else
    if ((wifi_netlink_link_state_get() >= WIFI_NETLINK_STATUS_LINKED)
        || (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_ROAMING)) {
        wifi_netlink_disconnect_req();
        wifi_netif_stop_dhcp();
    }

    while (retry) {
        ret = wifi_netlink_connect_req((uint8_t *)ssid, (uint8_t *)passwd);
        if (ret != 0) {
            platform_warn("wifi connect failed\r\n");
            retry--;
            continue;
        }

        for (i = 0; i < 20; i++) {
            if (wifi_netlink_link_state_get() == WIFI_NETLINK_STATUS_LINKED) {
                wifi_netif_start_dhcp();
                if (TRUE == wifi_netif_polling_dhcp()) {
                    wifi_netlink_ipaddr_set((u8*)wifi_netif_get_ip());
                    platform_warn("WIFI_CONNECT: ip configured, IP: %s\r\n", ip_ntoa(wifi_netif_get_ip()));
                    return 0;
                } else {
                    platform_warn("WIFI_CONNECT: get ip timeout!\r\n");
                    wifi_netif_stop_dhcp();
                    wifi_netlink_disconnect_req();
                    break;
                }
            } else {
                sys_ms_sleep(500);
            }
        }
        retry--;
    }
#endif

    return -1;
}
/*
range 0 1000
default 200
*/
int HAL_Awss_Get_Channelscan_Interval_Ms(void)
{
    return 200;
}
/*
range 0 1800000
default 180000
*/
int HAL_Awss_Get_Timeout_Interval_Ms(void)
{
    return 180000;
}

int HAL_Awss_Get_Encrypt_Type(void)
{
    return 3;
}

int HAL_Awss_Get_Conn_Encrypt_Type(void)
{
    char invalid_ds[DEVICE_SECRET_LEN + 1] = {0};
    char ds[DEVICE_SECRET_LEN + 1] = {0};

    HAL_GetDeviceSecret(ds);

    if (memcmp(invalid_ds, ds, sizeof(ds)) == 0)
        return 3;

    memset(invalid_ds, 0xff, sizeof(invalid_ds));
    if (memcmp(invalid_ds, ds, sizeof(ds)) == 0)
        return 3;

    return 4;
}

int HAL_Awss_Open_Ap(const char *ssid, const char *passwd, int beacon_interval, int hide)
{
    char buffer[256] = {0};
    int ret = -1;
    uint8_t channel = 1;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_ap_start((char *)ssid, NULL, channel, 0);
#else
    wifi_netif_stop_dhcp();

    wifi_netlink_ap_start(ssid, NULL, channel, 0);

    CONFIG_IP4_ADDR(&ipaddr);
    CONFIG_IP4_ADDR_NM(&netmask);
    CONFIG_IP4_ADDR_GW(&gw);

    wifi_netif_set_addr(&ipaddr, &netmask, &gw);

    if (!ap_dhcpd_started) {
        dhcpd_daemon();
        ap_dhcpd_started = 1;
    }

    wifi_netif_set_up();
#endif

    return 0;
}

int HAL_Awss_Close_Ap(void)
{
#ifdef CONFIG_WIFI_MANAGEMENT_TASK
    wifi_management_sta_start();
#else
    if (p_wifi_netlink->ap_started){
        /* Stop DHCPD */
        if (ap_dhcpd_started) {
            stop_dhcpd_daemon();
            ap_dhcpd_started = 0;
        }
        wifi_netlink_dev_close();
        wifi_netlink_dev_open();
    }
#endif
    return 0;
}

void HAL_Awss_Switch_Channel(char primary_channel, char secondary_channel, uint8_t bssid[ETH_ALEN])
{
    wifi_netlink_channel_set((uint32_t)primary_channel, CHANNEL_WIDTH_20, 0);
}

#endif /* CONFIG_ALICLOUD_SUPPORT */
