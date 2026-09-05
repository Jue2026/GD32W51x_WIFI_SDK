#include "app_cfg.h"

#if defined(CONFIG_ALICLOUD_SUPPORT)

#include <stdio.h>
#include <stdint.h>
#include "iot_import.h"
#include "nspe_region.h"
#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#else
#include "mbl_api.h"
#endif

#define __DEMO__

/* define according to user's flash layout */
#define ALICLOUD_FLASH_BLOCK_START      (0x1D6000)
#define ALICLOUD_FLASH_BLOCK_SIZE       0x2000 //8K
#define FLASH_END_OFFSET                0x200000 //2M

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_erase(uint32_t off_set, uint32_t size)
{
    uint32_t flash_off = 0;

#ifdef __DEMO__
    return 0;
#endif

    flash_off = off_set + ALICLOUD_FLASH_BLOCK_START;
    if (flash_off <= FLASH_END_OFFSET) {
        if (!mbl_flash_erase(flash_off, size))
            return 0;
    }

    return -1;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_write(uint32_t *off_set, const void *in_buf, uint32_t in_buf_len)
{
    uint32_t flash_off = 0;

#ifdef __DEMO__
    return 0;
#endif

    if (off_set == NULL)
        return -1;

    flash_off = *off_set + ALICLOUD_FLASH_BLOCK_START;
    if (flash_off <= FLASH_END_OFFSET) {
        if (!mbl_flash_write(flash_off, in_buf, in_buf_len)) {
            *off_set = *off_set + in_buf_len;
            return 0;
        }
    }

    return -2;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t aiot_al_kv_flash_read(uint32_t *off_set, void *out_buf, uint32_t out_buf_len)
{
    uint32_t flash_off = 0;

#ifdef __DEMO__
    return 0;
#endif

    if (off_set == NULL)
        return -1;

    flash_off = *off_set + ALICLOUD_FLASH_BLOCK_START;
    if (flash_off <= FLASH_END_OFFSET) {
        if (!mbl_flash_indirect_read(flash_off, out_buf, out_buf_len)) {
            *off_set = *off_set + out_buf_len;
            return 0;
        }
    }

    return -2;
}
#endif /* CONFIG_ALICLOUD_SUPPORT */
