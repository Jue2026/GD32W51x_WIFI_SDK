#include "app_cfg.h"

#if defined(CONFIG_ALICLOUD_SUPPORT)

#include <string.h>

#include "iot_import.h"
#include "iot_export.h"

#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "wrapper_os.h"

int HAL_Wifi_Enable_Mgmt_Frame_Filter(
    _IN_ uint32_t filter_mask,
    _IN_OPT_ uint8_t vendor_oui[3],
    _IN_ awss_wifi_mgmt_frame_cb_t callback)
{
    platform_warn("=> HAL_Wifi_Enable_Mgmt_Frame_Filter\r\n");

    wifi_netlink_promisc_mgmt_cb_set(filter_mask, callback);
    return 0;
}

int HAL_Wifi_Get_Ap_Info(char ssid[HAL_MAX_SSID_LEN], char passwd[HAL_MAX_PASSWD_LEN], uint8_t bssid[ETH_ALEN])
{
    wifi_netlink_linked_ap_info_get((uint8_t  *)ssid, (uint8_t  *)passwd, bssid);

    return 0;
}

#define HAL_IP_LEN (15 + 1)
uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
    uint8_t *ip;

    ip = (uint8_t *)wifi_netif_get_ip();
    snprintf(ip_str, HAL_IP_LEN, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return *(uint32_t *)ip;
}

char *HAL_Wifi_Get_Mac(char mac_str[HAL_MAC_LEN])
{
    uint8_t mac[6] = {0};

    sys_memcpy(mac, wifi_netif_get_hwaddr(), 6);

    snprintf(mac_str, HAL_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],
            mac[1], mac[2], mac[3], mac[4], mac[5]);

    return mac_str;
}

int HAL_Wifi_Scan(awss_wifi_scan_result_cb_t cb)
{
    platform_info("=> HAL_Wifi_Scan, TODO\r\n");
    return 0;
}

int HAL_Wifi_Send_80211_Raw_Frame(_IN_ enum HAL_Awss_Frame_Type type,
                                  _IN_ uint8_t *buffer, _IN_ int len)
{
    wifi_netlink_raw_send(buffer, len - 4, 0.0f);

    return 0;
}

#endif /* CONFIG_ALICLOUD_SUPPORT */
