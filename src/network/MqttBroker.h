#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <PicoMQTT.h>
#include "../core/DeviceState.h"
#include "MqttController.h"

class MqttBroker
{
public:
    void begin(DeviceState &state);
    void update(DeviceState &state);

    PicoMQTT::Server& getBroker() { return mqttBroker; }

private:
    PicoMQTT::Server mqttBroker;
    MqttController controller;
    bool mdnsStarted = false;

    void startMDNS();
};

void MqttBroker::begin(DeviceState &state)
{
    // Setup MQTT broker on default port 1883
    mqttBroker.begin();
    Serial0.println("[MQTT] Broker started on port 1883");

    // Initialize controller with broker reference
    controller.begin(mqttBroker);

    // Subscribe to command topics
    mqttBroker.subscribe(MqttController::TOPIC_CMD_MOTOR, [&state, this](const char* topic, const char* payload) {
        this->controller.processMotorCommand(state, payload);
    });

    mqttBroker.subscribe(MqttController::TOPIC_CMD_CONFIG, [&state, this](const char* topic, const char* payload) {
        this->controller.processConfigCommand(state, payload);
    });

    // Publish online status
    controller.publish(MqttController::TOPIC_STATUS, R"({"status":"online"})");

    Serial0.println("[MQTT] Subscriptions setup complete");

    startMDNS();
}

void MqttBroker::startMDNS()
{
    if (!mdnsStarted)
    {
        if (MDNS.begin("hub"))
        {
            mdnsStarted = true;
            Serial0.println("[mDNS] Responder started: hub.local");
            // Advertise MQTT service
            MDNS.addService("mqtt", "tcp", 1883);
            Serial0.println("[mDNS] Service advertised: mqtt on port 1883");
        }
        else
        {
            Serial0.println("[mDNS] Error setting up MDNS responder");
        }
    }
}

void MqttBroker::update(DeviceState &state)
{
    // Only run MQTT when WiFi is connected to save CPU
    if (WiFi.status() == WL_CONNECTED)
    {
        // MQTT broker loop
        mqttBroker.loop();

        // Update controller (telemetry publish)
        controller.update(state);

        // Start MDNS if not started
        if (!mdnsStarted)
        {
            startMDNS();
        }
    }
}
