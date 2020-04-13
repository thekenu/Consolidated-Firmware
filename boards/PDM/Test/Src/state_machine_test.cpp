#include "fff.h"
#include "gtest/gtest.h"

extern "C"
{
#include "App_SharedStateMachine.h"
#include "states/App_InitState.h"
#include "states/App_AirOpenState.h"
#include "states/App_AirClosedState.h"

    DEFINE_FFF_GLOBALS;
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
    FAKE_VALUE_FUNC(float, GetVbatMinVoltage);
    FAKE_VALUE_FUNC(float, GetVbatMaxVoltage);
    FAKE_VOID_FUNC(VbatErrorCallback, struct VoltageMonitor *);

    FAKE_VALUE_FUNC(float, Get24vAuxVoltage);
    FAKE_VALUE_FUNC(float, Get24vAuxMinVoltage);
    FAKE_VALUE_FUNC(float, Get24vAuxMaxVoltage);
    FAKE_VOID_FUNC(_24vAuxErrorCallback, struct VoltageMonitor *);

    FAKE_VALUE_FUNC(float, Get24vAccVoltage);
    FAKE_VALUE_FUNC(float, Get24vAccMinVoltage);
    FAKE_VALUE_FUNC(float, Get24vAccMaxVoltage);
    FAKE_VOID_FUNC(_24vAccErrorCallback, struct VoltageMonitor *);
}

class PdmStateMachineTest : public testing::Test
{
  protected:
    void SetUp() override
    {
        can_tx_interface = App_CanTx_Create(
            send_non_periodic_msg_PDM_STARTUP,
            send_non_periodic_msg_PDM_AIR_SHUTDOWN,
            send_non_periodic_msg_PDM_MOTOR_SHUTDOWN,
            send_non_periodic_msg_PDM_WATCHDOG_TIMEOUT);
        can_rx_interface     = App_CanRx_Create();
        vbat_voltage_monitor = App_VoltageMonitor_Create(
            GetVbatVoltage, GetVbatMinVoltage, GetVbatMaxVoltage,
            VbatErrorCallback);
        _24v_aux_voltage_monitor = App_VoltageMonitor_Create(
            Get24vAccVoltage, Get24vAccMinVoltage, Get24vAccMaxVoltage,
            _24vAuxErrorCallback);
        _24v_acc_voltage_monitor = App_VoltageMonitor_Create(
            Get24vAccVoltage, Get24vAccMinVoltage, Get24vAccMaxVoltage,
            _24vAccErrorCallback);
        world = App_PdmWorld_Create(
            can_tx_interface, can_rx_interface, vbat_voltage_monitor,
            _24v_aux_voltage_monitor, _24v_acc_voltage_monitor);

        // Default to starting the state machine in the `init` state
        state_machine =
            App_SharedStateMachine_Create(world, App_GetInitState());

        // Reset fake functions
        RESET_FAKE(send_non_periodic_msg_PDM_STARTUP);
        RESET_FAKE(send_non_periodic_msg_PDM_AIR_SHUTDOWN);
        RESET_FAKE(send_non_periodic_msg_PDM_MOTOR_SHUTDOWN);
        RESET_FAKE(send_non_periodic_msg_PDM_WATCHDOG_TIMEOUT);
        RESET_FAKE(GetVbatVoltage);
        RESET_FAKE(GetVbatMinVoltage);
        RESET_FAKE(GetVbatMaxVoltage);
        RESET_FAKE(VbatErrorCallback);
        RESET_FAKE(Get24vAuxVoltage);
        RESET_FAKE(Get24vAuxMinVoltage);
        RESET_FAKE(Get24vAuxMaxVoltage);
        RESET_FAKE(_24vAuxErrorCallback);
        RESET_FAKE(Get24vAccVoltage);
        RESET_FAKE(Get24vAccMinVoltage);
        RESET_FAKE(Get24vAccMaxVoltage);
        RESET_FAKE(_24vAccErrorCallback);
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

    void TearDown() override
    {
        ASSERT_TRUE(world != NULL);
        ASSERT_TRUE(can_tx_interface != NULL);
        ASSERT_TRUE(can_rx_interface != NULL);
        ASSERT_TRUE(vbat_voltage_monitor != NULL);
        ASSERT_TRUE(_24v_aux_voltage_monitor != NULL);
        ASSERT_TRUE(_24v_acc_voltage_monitor != NULL);
        ASSERT_TRUE(state_machine != NULL);

        App_PdmWorld_Destroy(world);
        App_CanTx_Destroy(can_tx_interface);
        App_CanRx_Destroy(can_rx_interface);
        App_VoltageMonitor_Destroy(vbat_voltage_monitor);
        App_VoltageMonitor_Destroy(_24v_aux_voltage_monitor);
        App_VoltageMonitor_Destroy(_24v_acc_voltage_monitor);
        App_SharedStateMachine_Destroy(state_machine);

        world                    = NULL;
        can_tx_interface         = NULL;
        can_rx_interface         = NULL;
        vbat_voltage_monitor     = NULL;
        _24v_aux_voltage_monitor = NULL;
        _24v_acc_voltage_monitor = NULL;
        state_machine            = NULL;
    }

    struct World *            world;
    struct PdmCanTxInterface *can_tx_interface;
    struct PdmCanRxInterface *can_rx_interface;
    struct VoltageMonitor *   vbat_voltage_monitor;
    struct VoltageMonitor *   _24v_aux_voltage_monitor;
    struct VoltageMonitor *   _24v_acc_voltage_monitor;
    struct StateMachine *     state_machine;
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
