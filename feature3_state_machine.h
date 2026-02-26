#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum
{
    BMS_STATE_IDLE = 0,
    BMS_STATE_READY,
    BMS_STATE_ERROR
} BMS_State;

void state_machine_init(void);
void BMS_force_error_state(void);
BMS_State BMS_get_state(void);
const char* BMS_get_state_name(void);
void BMS_set_idle_state(void);
void BMS_set_ready_state(void);

#endif