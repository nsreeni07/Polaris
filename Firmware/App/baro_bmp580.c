/* atltitude, is that how you spell alititude?*/
#include "baro_bmp580.h"
#include "app_config.h"
#include "board.h"
#include <math.h>


#define BMP_ADDR              (0x47 << 1)

#define REG_CHIP_ID           0x01
#define REG_TEMP_DATA_XLSB    0x1D  
#define REG_PRESS_DATA_XLSB   0x20  
#define REG_OSR_CONFIG        0x36   
#define REG_ODR_CONFIG        0x37   

#define CHIP_ID_VAL_PRIM      0x50
#define CHIP_ID_VAL_SEC       0x51   


#define OSR_PRESS_EN_BIT      (1u << 6)
#define OSR_T_1X               0x00
#define OSR_P_1X               (0x00 << 3)


#define PWR_MODE_STANDBY      0x00
#define PWR_MODE_NORMAL       0x01
#define PWR_MODE_FORCED       0x02
#define PWR_MODE_CONTINUOUS   0x03
#define DEEP_DISABLE_BIT      (1u << 7)  

static float s_ground_pa = SEA_LEVEL_PA_DEFAULT;

static int reg_read_multi(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, BMP_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 20) == HAL_OK ? 0 : -1;
}

static int reg_write(uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(&hi2c1, BMP_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 20) == HAL_OK ? 0 : -1;
}

int baro_init(void)
{
    uint8_t id;
    if (reg_read_multi(REG_CHIP_ID, &id, 1) != 0)
        return -1;
    if (id != CHIP_ID_VAL_PRIM && id != CHIP_ID_VAL_SEC)
        return -1;

    
    if (reg_write(REG_OSR_CONFIG, OSR_PRESS_EN_BIT | OSR_P_1X | OSR_T_1X) != 0)
        return -1;
    if (reg_write(REG_ODR_CONFIG, DEEP_DISABLE_BIT | PWR_MODE_NORMAL) != 0)
        return -1;

    HAL_Delay(5); /* let first conversion complete before first read */
    return 0;
}

static int read_pressure_pa(float *pa)
{
    uint8_t buf[3];
    if (reg_read_multi(REG_PRESS_DATA_XLSB, buf, 3) != 0)
        return -1;
    uint32_t raw = ((uint32_t)buf[2] << 16) | ((uint32_t)buf[1] << 8) | buf[0];
    *pa = (float)raw / 64.0f;   
    return 0;
}

static int read_temp_c(float *temp_c)
{
    uint8_t buf[3];
    if (reg_read_multi(REG_TEMP_DATA_XLSB, buf, 3) != 0)
        return -1;
   
    int32_t raw = (int32_t)(((uint32_t)((uint32_t)buf[2] << 16 |
                                         (uint32_t)buf[1] << 8 |
                                         buf[0]) << 8)) >> 8;
    *temp_c = (float)raw / 65536.0f;  
    return 0;
}

int baro_read(float *alt_m, float *temp_c)
{
    float pa;
    if (read_pressure_pa(&pa) != 0)
        return -1;
    if (read_temp_c(temp_c) != 0)
        return -1;
    *alt_m = 44330.0f * (1.0f - powf(pa / s_ground_pa, 0.1903f));
    return 0;
}

void baro_zero(void)
{
    float sum = 0.0f;
    const int n = 32;
    for (int i = 0; i < n; i++) {
        float pa;
        if (read_pressure_pa(&pa) == 0)
            sum += pa;
        HAL_Delay(1000 / BARO_HZ);
    }
    s_ground_pa = sum / n;
}