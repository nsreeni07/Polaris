#pragma once
#include "app_types.h"

int  imu_init(void);
int  imu_read(imu_sample_t *out);
void imu_calibrate_gyro(void);
