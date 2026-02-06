#pragma once
#include <Arduino.h>
#include <WiFi.h>

struct DeviceState
{
    // Network
    bool wifiConnected = false;
    bool apActive = false;
    String savedSsid = "";
    IPAddress localIP;
    bool mqttConnected = false;

    // Sensors
    int32_t encoderPos = 0;
    int16_t currentAdc = 0;

    // Motor
    int motorSpeed = 0;
};
