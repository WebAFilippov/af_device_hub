#pragma once
#include "../core/DeviceState.h"
#include "../core/Config.h"

class CurrentSensor
{
public:
    void begin();
    void update(DeviceState &state);

private:
    int last = -999;
};

void CurrentSensor::begin()
{
    analogReadResolution(Config::Current::ADC_RESOLUTION);
    analogSetAttenuation(ADC_11db);
}

void CurrentSensor::update(DeviceState &state)
{
    int val = analogRead(Config::Pins::CURRENT_ADC);
    if (abs(val - last) > Config::Current::ADC_THRESHOLD)
    {
        state.currentAdc = val;
        last = val;
    }
}
