#pragma once
#include "app_types.h"

void kalman_init(void);
void kalman_predict(float accel_ms2, float dt_s);
void kalman_correct_baro(float alt_m);
alt_state_t kalman_get_state(void);
