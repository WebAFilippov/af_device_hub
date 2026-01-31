#pragma once
#include <ESP32Encoder.h>
#include "../core/DeviceState.h"

class EncoderReader
{
public:
    void begin();
    void update(DeviceState &state);

private:
    ESP32Encoder encoder;
    int32_t lastPos = 0;
};


void EncoderReader::begin()
{
    pinMode(15, INPUT_PULLUP);
    pinMode(16, INPUT_PULLUP);

    encoder.attachHalfQuad(15, 16);
    encoder.setFilter(1023);
}

void EncoderReader::update(DeviceState &state)
{
    int32_t pos = encoder.getCount();
    if (pos != lastPos)
    {
        state.encoderPos = pos;
        lastPos = pos;
    }
}
