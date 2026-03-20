#include "feature3_state_machine.h"

static BMS_State current_state;

void state_machine_init(void)
{
    current_state = BMS_STATE_IDLE;
}


void BMS_force_error_state(void)
{
    current_state = BMS_STATE_ERROR;
}

BMS_State BMS_get_state(void)
{
    return current_state;
}

const char* BMS_get_state_name(void)
{
    switch (current_state)
    {
        case BMS_STATE_IDLE:
        return "IDLE";
        case BMS_STATE_READY:
        return "READY_TO_DRIVE";
        case BMS_STATE_ERROR:
        return "ERROR";
        default:
        return "UNKNOWN";
    }
}

void BMS_set_idle_state(void)
{
    current_state = BMS_STATE_IDLE;
}

void BMS_set_ready_state(void)
{
    if (current_state != BMS_STATE_ERROR)
    {
        current_state = BMS_STATE_READY;
    }
}