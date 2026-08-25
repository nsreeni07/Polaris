#pragma once
#include "app_types.h"

void fsm_init(void);
void fsm_tick(const imu_sample_t *s, float dt_s);
flight_state_e fsm_get_state(void);
int fsm_request_arm(void);   /* call from a ground command, not automatic */
