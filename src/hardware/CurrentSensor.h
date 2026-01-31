#pragma once
#include "../core/DeviceState.h"

class CurrentSensor
{
public:
    void begin();
    void update(DeviceState &state);

private:
    int last = -999;
    const int threshold = 20;
};

void CurrentSensor::begin()
{
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

void CurrentSensor::update(DeviceState &state)
{
    int val = analogRead(7);
    if (abs(val - last) > threshold)
    {
        state.currentAdc = val;
        last = val;
    }
}
