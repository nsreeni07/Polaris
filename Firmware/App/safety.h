#pragma once
#include "app_types.h"

void safety_init(void);            /* starts IWDG - call last in app_init */
void wdg_refresh(void);
int  safety_check_arm_gate(const fsm_snapshot_t *snap);   /* 1 = ok to arm */
