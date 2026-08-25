#pragma once
#include "app_types.h"

void tvc_init(void);
void tvc_set_gains(float kp, float ki, float kd);
void tvc_update(const att_state_t *att, float dt_s);   /* drives servos directly */
void tvc_disable(void);                                 /* centers and stops correcting */
