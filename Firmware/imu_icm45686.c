#include "imu_icm45686.h"
#include "app_config.h"
#include "board.h"

#define REG_WHO_AM_I     0x72
#define REG_PWR_MGMT0    0x10
#define REG_ACCEL_DATA_X 0x00
#define WHO_AM_I_VAL     0xE9

#define PWR_MODE_LOW_NOISE      0x3
#define PWR_MGMT0_ACCEL_GYRO_LN ((PWR_MODE_LOW_NOISE << 2) | PWR_MODE_LOW_NOISE)

#define SENSOR_STARTUP_DELAY_MS 60

volatile uint8_t g_imu_data_ready = 0;
static float s_gyro_bias[3];

static void cs_low(void)  { HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_RESET); }
static void cs_high(void) { HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_SET); }

static uint8_t reg_read(uint8_t reg)
{
    uint8_t tx[2] = { (uint8_t)(reg | 0x80), 0x00 };
    uint8_t rx[2] = { 0 };
    cs_low();
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 10);
    cs_high();
    return rx[1];
}

static void reg_write(uint8_t reg, uint8_t val)
{
    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    cs_low();
    HAL_SPI_Transmit(&hspi1, tx, 2, 10);
    cs_high();
}

static void burst_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t cmd = reg | 0x80;
    cs_low();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 10);
    HAL_SPI_Receive(&hspi1, buf, len, 10);
    cs_high();
}

int imu_init(void)
{
    cs_high();
    HAL_Delay(5);
    if (reg_read(REG_WHO_AM_I) != WHO_AM_I_VAL)
        return -1;

    reg_write(REG_PWR_MGMT0, PWR_MGMT0_ACCEL_GYRO_LN);
    HAL_Delay(SENSOR_STARTUP_DELAY_MS);
    s_gyro_bias[0] = s_gyro_bias[1] = s_gyro_bias[2] = 0.0f;
    return 0;
}

int imu_read(imu_sample_t *out)
{
    uint8_t buf[12];
    burst_read(REG_ACCEL_DATA_X, buf, 12);

    int16_t ax = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t ay = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t az = (int16_t)((buf[4] << 8) | buf[5]);
    int16_t gx = (int16_t)((buf[6] << 8) | buf[7]);
    int16_t gy = (int16_t)((buf[8] << 8) | buf[9]);
    int16_t gz = (int16_t)((buf[10] << 8) | buf[11]);

    out->t_ms = HAL_GetTick();
    out->ax = (float)ax / 32768.0f * ACCEL_FS_G;
    out->ay = (float)ay / 32768.0f * ACCEL_FS_G;
    out->az = (float)az / 32768.0f * ACCEL_FS_G;
    out->gx = (float)gx / 32768.0f * GYRO_FS_DPS - s_gyro_bias[0];
    out->gy = (float)gy / 32768.0f * GYRO_FS_DPS - s_gyro_bias[1];
    out->gz = (float)gz / 32768.0f * GYRO_FS_DPS - s_gyro_bias[2];
    return 0;
}

void imu_calibrate_gyro(void)
{
    float sum[3] = {0}, minv[3], maxv[3];
    imu_sample_t s;
    imu_read(&s);
    minv[0] = maxv[0] = s.gx;
    minv[1] = maxv[1] = s.gy;
    minv[2] = maxv[2] = s.gz;

    for (int i = 0; i < GYRO_CAL_SAMPLES; i++) {
        HAL_Delay(1000 / CTRL_HZ);
        imu_read(&s);
        sum[0] += s.gx; sum[1] += s.gy; sum[2] += s.gz;
        if (s.gx < minv[0]) minv[0] = s.gx; if (s.gx > maxv[0]) maxv[0] = s.gx;
        if (s.gy < minv[1]) minv[1] = s.gy; if (s.gy > maxv[1]) maxv[1] = s.gy;
        if (s.gz < minv[2]) minv[2] = s.gz; if (s.gz > maxv[2]) maxv[2] = s.gz;
    }

    if ((maxv[0] - minv[0]) > GYRO_CAL_MAX_PP_DPS ||
        (maxv[1] - minv[1]) > GYRO_CAL_MAX_PP_DPS ||
        (maxv[2] - minv[2]) > GYRO_CAL_MAX_PP_DPS)
        return;

    s_gyro_bias[0] = sum[0] / GYRO_CAL_SAMPLES;
    s_gyro_bias[1] = sum[1] / GYRO_CAL_SAMPLES;
    s_gyro_bias[2] = sum[2] / GYRO_CAL_SAMPLES;
}