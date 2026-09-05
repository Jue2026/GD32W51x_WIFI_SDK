/*!
    \file    wifi_netif.c
    \brief   Implementation of WiFi network interface layer.

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

#include "app_cfg.h"
#include "lwip/opt.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/prot/dhcp.h"
#include "netif/etharp.h"
#include "wlan_intf.h"
#include "wifi_netif.h"
#include "wrapper_os.h"
#include "ethernetif.h"
#include "debug_print.h"
#include "wifi_netlink.h"
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif

static struct netif wifi_netif;
static uint8_t ip_static_mode = 0;

void wifi_netif_open(void)
{
    uint8_t *mac;
#if LWIP_IPV6
    netif_add(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), NULL, &ethernetif_init, &tcpip_input);
    netif_create_ip6_linklocal_address(&wifi_netif, 1);
#if LWIP_IPV6_DHCP6
    dhcp6_enable_stateless(&wifi_netif);
#endif
#else
    netif_add(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY, NULL, &ethernetif_init, &tcpip_input);
#endif
    mac = wifi_netif_get_hwaddr();
    DEBUGPRINT("WiFi MAC address: "MAC_FMT"\r\n",MAC_ARG(mac));
    netif_set_default(&wifi_netif);
}

void wifi_netif_close(void)
{
    struct ethernetif *ethernet_if = (struct ethernetif *)(wifi_netif.state);

    wifi_netif.state = NULL;
    ethernet_if->adapter = NULL;

    wifi_ops_entry.wifi_close_func();

    ethernetif_deinit(ethernet_if);
    netifapi_dhcp_stop(&wifi_netif);
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY));
#else
    netifapi_netif_set_addr(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY);
#endif
    netifapi_netif_remove(&wifi_netif);
    dhcp_cleanup(&wifi_netif);
}

uint8_t wifi_netif_set_hwaddr(uint8_t *mac_addr)
{
    struct ethernetif *ethernet_if = (struct ethernetif *)(wifi_netif.state);

    if (wifi_ops_entry.wifi_set_mac_addr(mac_addr)) {
        sys_memcpy(wifi_netif.hwaddr, mac_addr, NETIF_MAX_HWADDR_LEN);
        sys_memcpy(ethernet_if->mac_addr, mac_addr, NETIF_MAX_HWADDR_LEN);
        return TRUE;
    } else {
        return FALSE;
    }

}

uint8_t *wifi_netif_get_hwaddr(void)
{
    return (uint8_t *)wifi_netif.hwaddr;
}

ip_addr_t *wifi_netif_get_ip(void)
{
    return &wifi_netif.ip_addr;
}

#if LWIP_IPV6
ip_addr_t *wifi_netif_get_ip6(uint8_t index)
{
    return &wifi_netif.ip6_addr[index];
}

void wifi_netif_set_ip6addr_invalid(void)
{
    wifi_netif.ip6_addr_state[1] = IP6_ADDR_INVALID;
    ip6_addr_set_zero(&wifi_netif.ip6_addr[1].u_addr.ip6);
}
#endif

void wifi_netif_set_ip(ip4_addr_t *ip, ip4_addr_t *netmask, ip4_addr_t *gw)
{
    netifapi_netif_set_addr(&wifi_netif, ip, netmask, gw);
    wifi_ops_entry.wifi_set_ipaddr_func((uint8_t *)ip);
}

void wifi_netif_set_dns(uint8_t numdns, ip4_addr_t *dns_server)
{
#if LWIP_DNS
    ip_addr_t dnsserver;

    ip_addr_copy_from_ip4(dnsserver, *dns_server);

    dns_setserver(numdns, &dnsserver);
#else
    DEBUGPRINT("NOT enable LWIP_DNS.\r\n");
#endif
}

void wifi_netif_get_dns(void)
{
#if LWIP_DNS
    const ip_addr_t *dns_server;
    int i = 0;

    for (i = 0; i < DNS_MAX_SERVERS; i++) {
        dns_server = dns_getserver(i);
#if LWIP_IPV6
        if (IP_IS_V6_VAL(*dns_server)) {
            DEBUGPRINT("dns[%d]: [%s]\r\n", i, ip6addr_ntoa(ip_2_ip6(dns_server)));
        } else
#endif /* LWIP_IPV6 */
        {
            DEBUGPRINT("dns[%d]: ["IP_FMT"]\r\n", i, IP_ARG(ip_2_ip4(dns_server)->addr));
        }
    }
#else
    DEBUGPRINT("NOT enable LWIP_DNS.\r\n");
#endif /* LWIP_DNS */
}

ip_addr_t *wifi_netif_get_gw(void)
{
    return &wifi_netif.gw;
}

ip_addr_t *wifi_netif_get_netmask(void)
{
    return &wifi_netif.netmask;
}

void wifi_netif_set_addr(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(ip), ip_2_ip4(mask), ip_2_ip4(gw));
#else
    netifapi_netif_set_addr(&wifi_netif, ip, mask, gw);
#endif
}

void wifi_netif_set_up(void)
{
    netifapi_netif_set_up(&wifi_netif);
}

void wifi_netif_set_down(void)
{
    netifapi_netif_set_down(&wifi_netif);
}

int32_t wifi_netif_is_ipv4_got(void)
{
    if (!ip_addr_isany(&wifi_netif.ip_addr) &&
        ((netif_dhcp_data(&wifi_netif)->state == DHCP_STATE_BOUND) || wifi_netif_is_static_ip_mode())) {
        return TRUE;
    } else {
        return FALSE;
    }
}

#if LWIP_IPV6
int32_t wifi_netif_is_ipv6_got(void)
{
    if (!ip6_addr_isany(&wifi_netif.ip6_addr[1].u_addr.ip6) && wifi_netif.ip6_addr_state[1] != IP6_ADDR_INVALID) {
        return TRUE;
    } else {
        return FALSE;
    }
}
#endif

void wifi_netif_start_dhcp(void)
{
    //DEBUGPRINT("WiFi Netif: Start DHCP\n");

    netifapi_netif_set_up(&wifi_netif);
    if (wifi_netif_is_static_ip_mode())
        return;
    netifapi_dhcp_start(&wifi_netif);
}

int32_t wifi_netif_polling_dhcp(void)
{
    uint32_t i;
    if (wifi_netif_is_static_ip_mode())
        return TRUE;
    for (i=0; i < 200; i++) {
        if ((i % 40) == 0) {
            //DEBUGPRINT("WiFi Netif: DHCP doing (state = %d)...\n", netif_dhcp_data(&wifi_netif)->state);
        }
        sys_ms_sleep(100);
        if (netif_dhcp_data(&wifi_netif)->state == DHCP_STATE_BOUND) {
            return TRUE;
        }
    }
    return FALSE;
}

void wifi_netif_stop_dhcp(void)
{
    //DEBUGPRINT("WiFi Netif: Stop DHCP\n");
     if(wifi_netif_is_static_ip_mode()) {
         return;
     }
    netifapi_dhcp_stop(&wifi_netif);
    netifapi_netif_set_down(&wifi_netif);
#if LWIP_IPV6
    netifapi_netif_set_addr(&wifi_netif, ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY), ip_2_ip4(IP_ADDR_ANY));
#else
    netifapi_netif_set_addr(&wifi_netif, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY);
#endif
}

void wifi_netif_set_ip_mode(uint8_t ip_mode)
{
    ip_static_mode = ip_mode;
}

int32_t wifi_netif_is_static_ip_mode(void)
{
    return ((ip_static_mode == IP_MODE_STATIC) ? 1 : 0);
}

struct netif *wifi_netif_get(void)
{
    return &wifi_netif;
}
