#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum
{
    IDLE = 0,
    READY_TO_DRIVE,
    ERROR
} BMS_State;

void StateMachine_Init(void);
void BMS_ForceErrorState(void);
void BMS_SetState(BMS_State newState);
BMS_State BMS_GetState(void);
const char* BMS_GetStateName(void);
void StateMachine_Process(void);

#endif