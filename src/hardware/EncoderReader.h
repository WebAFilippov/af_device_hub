#pragma once
#include <ESP32Encoder.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"

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
    pinMode(Config::Pins::ENCODER_A, INPUT_PULLUP);
    pinMode(Config::Pins::ENCODER_B, INPUT_PULLUP);

    encoder.attachHalfQuad(Config::Pins::ENCODER_A, Config::Pins::ENCODER_B);
    encoder.setFilter(Config::Encoder::FILTER_VALUE);
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
