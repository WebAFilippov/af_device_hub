#pragma once
#include "../core/DeviceState.h"

#include "../hardware/EncoderReader.h"
#include "../hardware/Buttons.h"
#include "../hardware/MotorController.h"
#include "../hardware/CurrentSensor.h"

#include "../network/WebServer.h"
#include "../network/WiFiManager.h"
#include "../network/MqttBroker.h"

class App
{
public:
    void setup();
    void loop();

private:
    DeviceState state;

    WiFiManager wifi;
    WebServer web;
    MqttBroker mqtt;

    EncoderReader encoder;
    Buttons buttons;
    MotorController motor;
    CurrentSensor current;
};

void App::setup()
{
    Serial0.begin(115200);

    wifi.begin(state);
    web.begin(state);
    mqtt.begin(state);

    encoder.begin();
    buttons.begin();
    motor.begin();
    current.begin();
}

void App::loop()
{
    wifi.update(state);
    web.update(state);
    mqtt.update(state);

    buttons.update(state, wifi);
    encoder.update(state);
    current.update(state);

    motor.update(state);
}
