/* Last 128K of flash (sector 7, 0x08060000) - kept out of .text by
 * the linker script. One record, CRC-checked, defaults on failure. */
#include "param_store.h"
#include "app_config.h"
#include "stm32f7xx_hal.h"
#include <string.h>

#define PARAM_ADDR   0x08060000U
#define PARAM_SECTOR FLASH_SECTOR_7

static uint32_t crc32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ (0xEDB88320u & (-(int32_t)(crc & 1)));
    }
    return ~crc;
}

static void defaults(params_t *p)
{
    p->tvc_kp = TVC_KP;
    p->tvc_ki = TVC_KI;
    p->tvc_kd = TVC_KD;
    p->baro_ground_pa = SEA_LEVEL_PA_DEFAULT;
}

void param_load(params_t *out)
{
    const params_t *flash_p = (const params_t *)PARAM_ADDR;
    params_t tmp = *flash_p;
    uint32_t crc = crc32((const uint8_t *)&tmp, sizeof(tmp) - sizeof(tmp.crc));

    if (crc == tmp.crc)
        *out = tmp;
    else
        defaults(out);
}

int param_save(const params_t *in)
{
    params_t tmp = *in;
    tmp.crc = crc32((const uint8_t *)&tmp, sizeof(tmp) - sizeof(tmp.crc));

    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Sector = PARAM_SECTOR,
        .NbSectors = 1,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3,
    };
    uint32_t err;
    if (HAL_FLASHEx_Erase(&erase, &err) != HAL_OK) {
        HAL_FLASH_Lock();
        return -1;
    }

    const uint32_t *src = (const uint32_t *)&tmp;
    uint32_t words = (sizeof(tmp) + 3) / 4;
    for (uint32_t i = 0; i < words; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, PARAM_ADDR + i * 4, src[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
    }
    HAL_FLASH_Lock();
    return 0;
}
