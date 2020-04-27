#pragma once

#include <stdbool.h>
#include "App_FaultManagerConfig.h"

#ifndef FSM_CAN
#error "Please define FSM_CAN"
#endif

#ifndef DCM_CAN
#error "Please define DCM_CAN"
#endif

#ifndef PDM_CAN
#error "Please define PDM_CAN"
#endif

#ifndef DIM_CAN
#error "Please define DIM_CAN"
#endif

#ifndef BMS_CAN
#error "Please define BMS_CAN"
#endif

void App_FaultManager_Init(
    void *fsm_can,
    void *dcm_can,
    void *pdm_can,
    void *dim_can,
    void *bms_can);
bool App_FaultManager_HasFsmCriticalFault(void);
bool App_FaultManager_HasDcmCriticalFault(void);
bool App_FaultManager_HasPdmCriticalFault(void);
bool App_FaultManager_HasDimCriticalFault(void);
bool App_FaultManager_HasBmsCriticalFault(void);
bool App_FaultManager_HasCriticalFault(void);
