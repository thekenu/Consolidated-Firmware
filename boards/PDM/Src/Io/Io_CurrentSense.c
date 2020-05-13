#include "Io_Adc.h"
#include "Io_CurrentSense.h"

// TODO: Fix hard-coded values once the ADC is configured to read load-switches

float Io_CurrentSense_GetAux1Current(void)
{
    return Io_Adc_GetChannel2Voltage() * 100.f;
}

float Io_CurrentSense_GetAux2Current(void)
{
    return Io_Adc_GetChannel7Voltage() * 100.f;
}

float Io_CurrentSense_GetLeftInverterCurrent(void)
{
    return Io_Adc_GetChannel8Voltage() * 100.f;
}

float Io_CurrentSense_GetRightInverterCurrent(void)
{
    return Io_Adc_GetChannel9Voltage() * 100.f;
}

float Io_CurrentSense_GetEnergyMeterCurrent(void)
{
    return 0.5f;
}

float Io_CurrentSense_GetCanCurrent(void)
{
    return 0.5f;
}

float Io_CurrentSense_GetAirShutdownCurrent(void)
{
    return 0.5f;
}
