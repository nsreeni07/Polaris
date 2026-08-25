#pragma once
#include "app_types.h"

void ahrs_init(void);
void ahrs_update(const imu_sample_t *s, float dt_s);
void ahrs_notify_launch(void);
att_state_t ahrs_get_attitude(void);
