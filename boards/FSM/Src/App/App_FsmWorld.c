#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "App_FsmWorld.h"

#define MAX_NUM_SIGNALS 5

struct FsmWorld
{
    struct FsmCanTxInterface *can_tx_interface;
    struct FsmCanRxInterface *can_rx_interface;
    struct HeartbeatMonitor * heartbeat_monitor;
    struct InRangeCheck *     primary_flow_rate_in_range_check;
    struct InRangeCheck *     secondary_flow_rate_in_range_check;
    struct InRangeCheck *     left_wheel_speed_in_range_check;
    struct InRangeCheck *     right_wheel_speed_in_range_check;
    struct InRangeCheck *     steering_angle_in_range_check;
    struct InRangeCheck *     brake_pressure_in_range_check;
    struct BinaryStatus *     brake_actuation_status;
    struct RgbLedSequence *   rgb_led_sequence;
    struct
    {
        struct Signal *signals[MAX_NUM_SIGNALS];
        uint32_t       num_signals;
    };
};

struct FsmWorld *App_FsmWorld_Create(
    struct FsmCanTxInterface *const can_tx_interface,
    struct FsmCanRxInterface *const can_rx_interface,
    struct HeartbeatMonitor *const  heartbeat_monitor,
    struct InRangeCheck *const      primary_flow_rate_in_range_check,
    struct InRangeCheck *const      secondary_flow_rate_in_range_check,
    struct InRangeCheck *const      left_wheel_speed_in_range_check,
    struct InRangeCheck *const      right_wheel_speed_in_range_check,
    struct InRangeCheck *const      steering_angle_in_range_check,
    struct InRangeCheck *const      brake_pressure_in_range_check,
    struct BinaryStatus *const      brake_actuation_status,
    struct RgbLedSequence *const    rgb_led_sequence)
{
    struct FsmWorld *world = (struct FsmWorld *)malloc(sizeof(struct FsmWorld));
    assert(world != NULL);

    world->can_tx_interface                 = can_tx_interface;
    world->can_rx_interface                 = can_rx_interface;
    world->heartbeat_monitor                = heartbeat_monitor;
    world->primary_flow_rate_in_range_check = primary_flow_rate_in_range_check;
    world->secondary_flow_rate_in_range_check =
        secondary_flow_rate_in_range_check;
    world->left_wheel_speed_in_range_check  = left_wheel_speed_in_range_check;
    world->right_wheel_speed_in_range_check = right_wheel_speed_in_range_check;
    world->steering_angle_in_range_check    = steering_angle_in_range_check;
    world->brake_pressure_in_range_check    = brake_pressure_in_range_check;
    world->brake_actuation_status           = brake_actuation_status;
    world->rgb_led_sequence                 = rgb_led_sequence;

    for (size_t i = 0; i < MAX_NUM_SIGNALS; i++)
    {
        world->signals[0] = NULL;
    }
    world->num_signals = 0;

    return world;
}

void App_FsmWorld_Destroy(struct FsmWorld *world)
{
    free(world);
}

struct FsmCanTxInterface *
    App_FsmWorld_GetCanTx(const struct FsmWorld *const world)
{
    return world->can_tx_interface;
}

struct FsmCanRxInterface *
    App_FsmWorld_GetCanRx(const struct FsmWorld *const world)
{
    return world->can_rx_interface;
}

struct HeartbeatMonitor *
    App_FsmWorld_GetHeartbeatMonitor(const struct FsmWorld *const world)
{
    return world->heartbeat_monitor;
}

struct InRangeCheck *App_FsmWorld_GetPrimaryFlowRateInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->primary_flow_rate_in_range_check;
}

struct InRangeCheck *App_FsmWorld_GetSecondaryFlowRateInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->secondary_flow_rate_in_range_check;
}

struct InRangeCheck *App_FsmWorld_GetLeftWheelSpeedInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->left_wheel_speed_in_range_check;
}

struct InRangeCheck *App_FsmWorld_GetRightWheelSpeedInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->right_wheel_speed_in_range_check;
}

struct InRangeCheck *App_FsmWorld_GetSteeringAngleInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->steering_angle_in_range_check;
}

struct InRangeCheck *App_FsmWorld_GetBrakePressureInRangeCheck(
    const struct FsmWorld *const world)
{
    return world->brake_pressure_in_range_check;
}

struct BinaryStatus *
    App_FsmWorld_GetBrakeActuationStatus(const struct FsmWorld *const world)
{
    return world->brake_actuation_status;
}

struct RgbLedSequence *
    App_FsmWorld_GetRgbLedSequence(const struct FsmWorld *const world)
{
    return world->rgb_led_sequence;
}

void App_FsmWorld_RegisterSignal(struct FsmWorld *world, struct Signal *signal)
{
    assert(world->num_signals < MAX_NUM_SIGNALS);
    world->signals[world->num_signals] = signal;
    world->num_signals++;
}

void App_FsmWorld_UpdateSignals(
    const struct FsmWorld *world,
    uint32_t               current_time_ms)
{
    for (size_t i = 0; i < world->num_signals; i++)
    {
        App_SharedSignal_Update(world->signals[i], current_time_ms);
    }
}
