#include "app_cfg.h"

#if defined(CONFIG_ALICLOUD_SUPPORT)

#include "iot_import.h"

#include "wrapper_os.h"
#include "nspe_region.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif


struct hal_ota_info {
    uint8_t running_idx;            // cur running image index
    uint32_t img_start_addr;        // flash start address to be written

    uint32_t max_img_len;
    uint32_t g_firmware_offset;     // flash offset to be written
};

struct hal_ota_info hal_ota_info_inst;

void HAL_Firmware_Persistence_Start(void)
{
    int32_t res = -1;
    uint8_t running_idx;
    struct hal_ota_info *hal_ota_info_ptr = &hal_ota_info_inst;

    platform_info("OTA start...\r\n");

    sys_memset(hal_ota_info_ptr, 0, sizeof(struct hal_ota_info));

    res = mbl_sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &running_idx);
    if (res < 0) {
        platform_err("OTA get running idx failed! (res = %d)\r\n", res);
        return;
    }

    hal_ota_info_ptr->running_idx = running_idx;
    if (running_idx == IMAGE_0) {
        hal_ota_info_ptr->img_start_addr = RE_IMG_1_PROT_OFFSET;
        hal_ota_info_ptr->max_img_len = RE_IMG_1_END_OFFSET - RE_IMG_1_PROT_OFFSET;
    } else {
        hal_ota_info_ptr->img_start_addr = RE_IMG_0_PROT_OFFSET;
        hal_ota_info_ptr->max_img_len = RE_IMG_1_PROT_OFFSET - RE_IMG_0_PROT_OFFSET;
    }

    res = mbl_flash_erase(hal_ota_info_ptr->img_start_addr, hal_ota_info_ptr->max_img_len);
    if (res < 0) {
        platform_err("OTA flash erase failed (res = %d)\r\n", res);
    }
}

int HAL_Firmware_Persistence_Stop(void)
{
    int32_t res = -1;
    struct hal_ota_info *hal_ota_info_ptr = &hal_ota_info_inst;

    res = mbl_sys_img_flag_set(hal_ota_info_ptr->running_idx, (IMG_FLAG_IA_MASK | IMG_FLAG_NEWER_MASK), (IMG_FLAG_IA_OK | IMG_FLAG_OLDER));
    res |= mbl_sys_img_flag_set(!(hal_ota_info_ptr->running_idx), (IMG_FLAG_IA_MASK | IMG_FLAG_VERIFY_MASK | IMG_FLAG_NEWER_MASK), 0);
    res |= mbl_sys_img_flag_set(!(hal_ota_info_ptr->running_idx), IMG_FLAG_NEWER_MASK, IMG_FLAG_NEWER);

    if (res != 0) {
        platform_err("OTA set image status failed! (res = %d)\r\n", res);
        return -1;
    }

    hal_ota_info_ptr->img_start_addr = 0;
    hal_ota_info_ptr->g_firmware_offset = 0;
    platform_info("OTA finish... Please reboot now.\r\n");
    return 0;
}

int HAL_Firmware_Persistence_Write(char *buffer, uint32_t length)
{
    int32_t res = -1;
    struct hal_ota_info *hal_ota_info_ptr = &hal_ota_info_inst;

    if (hal_ota_info_ptr->img_start_addr == 0) {
        platform_err("OTA is not started yet!\r\n");
        return -1;
    }

    if ((hal_ota_info_ptr->g_firmware_offset + length) > hal_ota_info_ptr->max_img_len) {
        platform_err("OTA firmware size overflow!\r\n");
        res = -2;
        goto exit;
    }

    res = mbl_flash_write((hal_ota_info_ptr->img_start_addr + hal_ota_info_ptr->g_firmware_offset), (uint8_t *)buffer, length);
    if (res < 0) {
        platform_err("OTA flash write failed!\r\n");
        res = -3;
        goto exit;
    }

    hal_ota_info_ptr->g_firmware_offset += length;

//    platform_err("OTA...writing at 0x%08x\r\n", hal_ota_info_ptr->g_firmware_offset);
    return 0;

exit:
    return res;
}

#endif /* CONFIG_ALICLOUD_SUPPORT */
