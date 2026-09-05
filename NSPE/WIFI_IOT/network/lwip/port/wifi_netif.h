/*!
    \file    wifi_netif.h
    \brief   Header file of declaration of WiFi network interface layer.

    \version 2023-07-20, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

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

#ifndef __WIFI_NETIF_H__
#define __WIFI_NETIF_H__

#include "app_type.h"
#include "lwip/netif.h"

void wifi_netif_open(void);
void wifi_netif_close(void);
uint8_t wifi_netif_set_hwaddr(uint8_t *mac_addr);
uint8_t *wifi_netif_get_hwaddr(void);
ip_addr_t *wifi_netif_get_ip(void);
#if LWIP_IPV6
ip_addr_t *wifi_netif_get_ip6(uint8_t index);
void wifi_netif_set_ip6addr_invalid(void);
#endif
void wifi_netif_set_ip(ip4_addr_t *ip, ip4_addr_t *netmask, ip4_addr_t *gw);
void wifi_netif_set_dns(uint8_t numdns, ip4_addr_t *dns_server);
void wifi_netif_get_dns(void);
ip_addr_t *wifi_netif_get_gw(void);
ip_addr_t *wifi_netif_get_netmask(void);
void wifi_netif_set_addr(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
void wifi_netif_set_up(void);
void wifi_netif_set_down(void);
int32_t wifi_netif_is_ipv4_got(void);
#if LWIP_IPV6
int32_t wifi_netif_is_ipv6_got(void);
#endif
void wifi_netif_start_dhcp(void);
int32_t wifi_netif_polling_dhcp(void);
void wifi_netif_stop_dhcp(void);
void wifi_netif_set_ip_mode(uint8_t ip_mode);
int32_t wifi_netif_is_static_ip_mode(void);
struct netif *wifi_netif_get(void);

#endif  // __WIFI_NETIF_H__
