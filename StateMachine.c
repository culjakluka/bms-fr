#include <stdio.h>
#include "StateMachine.h"

static BMS_State currentState;

void StateMachine_Init(void)
{
    currentState = IDLE;
}

void BMS_SetState(BMS_State newState)
{
    if (newState <= ERROR && newState >= IDLE)
    {
        currentState = newState;
    }
}

void BMS_ForceErrorState()
{
    if (currentState != IDLE)
    {
        currentState = ERROR;
    }
}

BMS_State BMS_GetState(void)
{
    return currentState;
}

const char* BMS_GetStateName(void)
{
    switch (currentState)
    {
        case IDLE:
            return "IDLE";
        case READY_TO_DRIVE:
            return "READY_TO_DRIVE";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void StateMachine_Process()
{
    switch (currentState)
    {
        case IDLE:
            //spreman za punjenje
            break;
        case READY_TO_DRIVE:
            //spreman za trošenje
            break;
        case ERROR:
            //error stanje
            break;
        default:
            //unknown stanje
            break;
    }
}