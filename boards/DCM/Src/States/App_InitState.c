#include "States/App_InitState.h"

static State_t *init_state;

State_t **App_InitState_GetStateHandlePointer(void)
{
    return &init_state;
}

State_t *App_InitState_GetStateHandle(void)
{
    return init_state;
}

void App_InitState_EntryAction(void) {}

void App_InitState_ExitAction(void) {}

void App_InitState_StateAction(void) {}