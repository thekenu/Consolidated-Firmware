#include "Test_Pdm.h"

extern "C"
{
#include "App_SharedHeartbeatMonitor.h"
#include "App_SharedStateMachine.h"
#include "states/App_InitState.h"
#include "states/App_AirOpenState.h"
#include "states/App_AirClosedState.h"
}

FAKE_VOID_FUNC(
    send_non_periodic_msg_PDM_STARTUP,
    struct CanMsgs_pdm_startup_t *);
FAKE_VOID_FUNC(
    send_non_periodic_msg_PDM_AIR_SHUTDOWN,
    struct CanMsgs_pdm_air_shutdown_t *);
FAKE_VOID_FUNC(
    send_non_periodic_msg_PDM_MOTOR_SHUTDOWN,
    struct CanMsgs_pdm_motor_shutdown_t *);
FAKE_VOID_FUNC(
    send_non_periodic_msg_PDM_WATCHDOG_TIMEOUT,
    struct CanMsgs_pdm_watchdog_timeout_t *);

FAKE_VALUE_FUNC(float, GetVbatVoltage);
FAKE_VOID_FUNC(VbatErrorCallback, struct InRangeCheck *);

FAKE_VALUE_FUNC(float, Get24vAuxVoltage);
FAKE_VOID_FUNC(_24vAuxErrorCallback, struct InRangeCheck *);

FAKE_VALUE_FUNC(float, Get24vAccVoltage);
FAKE_VOID_FUNC(_24vAccErrorCallback, struct InRangeCheck *);

FAKE_VALUE_FUNC(uint32_t, get_current_ms);
FAKE_VOID_FUNC(
    heartbeat_timeout_callback,
    enum HeartbeatOneHot,
    enum HeartbeatOneHot);

class PdmStateMachineTest : public testing::Test
{
  protected:
    void SetUp() override
    {
        constexpr uint32_t DEFAULT_HEARTBEAT_TIMEOUT_PERIOD_MS = 500U;
        constexpr enum HeartbeatOneHot DEFAULT_HEARTBEAT_BOARDS_TO_CHECK =
            BMS_HEARTBEAT_ONE_HOT;

        can_tx_interface = App_CanTx_Create(
            send_non_periodic_msg_PDM_STARTUP,
            send_non_periodic_msg_PDM_AIR_SHUTDOWN,
            send_non_periodic_msg_PDM_MOTOR_SHUTDOWN,
            send_non_periodic_msg_PDM_WATCHDOG_TIMEOUT);
        can_rx_interface   = App_CanRx_Create();
        vbat_voltage_check = App_InRangeCheck_Create(
            GetVbatVoltage, 22.0f, 26.0f, VbatErrorCallback);
        _24v_aux_voltage_check = App_InRangeCheck_Create(
            Get24vAccVoltage, 22.0f, 26.0f, _24vAuxErrorCallback);
        _24v_acc_voltage_check = App_InRangeCheck_Create(
            Get24vAccVoltage, 22.0f, 26.0f, _24vAccErrorCallback);
        heartbeat_check = App_SharedHeartbeatMonitor_Create(
            get_current_ms, DEFAULT_HEARTBEAT_TIMEOUT_PERIOD_MS,
            DEFAULT_HEARTBEAT_BOARDS_TO_CHECK, heartbeat_timeout_callback);
        world = App_PdmWorld_Create(
            can_tx_interface, can_rx_interface, vbat_voltage_check,
            _24v_aux_voltage_check, _24v_acc_voltage_check, heartbeat_check);

        // Default to starting the state machine in the `init` state
        state_machine =
            App_SharedStateMachine_Create(world, App_GetInitState());

        // Reset fake functions
        RESET_FAKE(send_non_periodic_msg_PDM_STARTUP);
        RESET_FAKE(send_non_periodic_msg_PDM_AIR_SHUTDOWN);
        RESET_FAKE(send_non_periodic_msg_PDM_MOTOR_SHUTDOWN);
        RESET_FAKE(send_non_periodic_msg_PDM_WATCHDOG_TIMEOUT);
        RESET_FAKE(GetVbatVoltage);
        RESET_FAKE(VbatErrorCallback);
        RESET_FAKE(Get24vAuxVoltage);
        RESET_FAKE(_24vAuxErrorCallback);
        RESET_FAKE(Get24vAccVoltage);
        RESET_FAKE(_24vAccErrorCallback);
        RESET_FAKE(get_current_ms);
        RESET_FAKE(heartbeat_timeout_callback);
    }

    void TearDown() override
    {
        ASSERT_TRUE(world != NULL);
        ASSERT_TRUE(can_tx_interface != NULL);
        ASSERT_TRUE(can_rx_interface != NULL);
        ASSERT_TRUE(vbat_voltage_check != NULL);
        ASSERT_TRUE(_24v_aux_voltage_check != NULL);
        ASSERT_TRUE(_24v_acc_voltage_check != NULL);
        ASSERT_TRUE(state_machine != NULL);
        ASSERT_TRUE(heartbeat_check != NULL);

        App_PdmWorld_Destroy(world);
        App_CanTx_Destroy(can_tx_interface);
        App_CanRx_Destroy(can_rx_interface);
        App_InRangeCheck_Destroy(vbat_voltage_check);
        App_InRangeCheck_Destroy(_24v_aux_voltage_check);
        App_InRangeCheck_Destroy(_24v_acc_voltage_check);
        App_SharedStateMachine_Destroy(state_machine);
        App_SharedHeartbeatMonitor_Destroy(heartbeat_check);

        world                  = NULL;
        can_tx_interface       = NULL;
        can_rx_interface       = NULL;
        vbat_voltage_check     = NULL;
        _24v_aux_voltage_check = NULL;
        _24v_acc_voltage_check = NULL;
        state_machine          = NULL;
        heartbeat_check        = NULL;
    }

    void SetInitialState(const struct State *const initial_state)
    {
        App_SharedStateMachine_Destroy(state_machine);
        state_machine = App_SharedStateMachine_Create(world, initial_state);
        ASSERT_TRUE(state_machine != NULL);
        ASSERT_EQ(
            initial_state,
            App_SharedStateMachine_GetCurrentState(state_machine));
    }

    struct World *            world;
    struct PdmCanTxInterface *can_tx_interface;
    struct PdmCanRxInterface *can_rx_interface;
    struct InRangeCheck *     vbat_voltage_check;
    struct InRangeCheck *     _24v_aux_voltage_check;
    struct InRangeCheck *     _24v_acc_voltage_check;
    struct StateMachine *     state_machine;
    struct HeartbeatMonitor * heartbeat_check;
};

TEST_F(PdmStateMachineTest, check_init_state_is_broadcasted_over_can)
{
    SetInitialState(App_GetInitState());

    EXPECT_EQ(
        CANMSGS_PDM_STATE_MACHINE_STATE_INIT_CHOICE,
        App_CanTx_GetPeriodicSignal_STATE(can_tx_interface));
}

TEST_F(PdmStateMachineTest, check_air_open_state_is_broadcasted_over_can)
{
    SetInitialState(App_GetAirOpenState());

    EXPECT_EQ(
        CANMSGS_PDM_STATE_MACHINE_STATE_AIR_OPEN_CHOICE,
        App_CanTx_GetPeriodicSignal_STATE(can_tx_interface));
}

TEST_F(PdmStateMachineTest, check_air_closed_state_is_broadcasted_over_can)
{
    SetInitialState(App_GetAirClosedState());

    EXPECT_EQ(
        CANMSGS_PDM_STATE_MACHINE_STATE_AIR_CLOSED_CHOICE,
        App_CanTx_GetPeriodicSignal_STATE(can_tx_interface));
}
