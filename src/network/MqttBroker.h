#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <PicoMQTT.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"
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
    // Setup MQTT broker
    mqttBroker.begin();
    Serial0.println(Config::Debug::LOG_MQTT " Broker started on port " + String(Config::Mqtt::PORT));

    // Initialize controller with broker reference
    controller.begin(mqttBroker);

    // Subscribe to command topics
    mqttBroker.subscribe(Config::Mqtt::TOPIC_CMD_MOTOR, [&state, this](const char* topic, const char* payload) {
        this->controller.processMotorCommand(state, payload);
    });

    mqttBroker.subscribe(Config::Mqtt::TOPIC_CMD_CONFIG, [&state, this](const char* topic, const char* payload) {
        this->controller.processConfigCommand(state, payload);
    });

    // Publish online status
    controller.publish(Config::Mqtt::TOPIC_STATUS, R"({"status":"online"})");

    Serial0.println(Config::Debug::LOG_MQTT " Subscriptions setup complete");

    startMDNS();
}

void MqttBroker::startMDNS()
{
    if (!mdnsStarted)
    {
        if (MDNS.begin(Config::Mqtt::MDNS_HOSTNAME))
        {
            mdnsStarted = true;
            Serial0.println("[mDNS] Responder started: " + String(Config::Mqtt::MDNS_HOSTNAME) + ".local");
            // Advertise MQTT service
            MDNS.addService(Config::Mqtt::MDNS_SERVICE, Config::Mqtt::MDNS_PROTOCOL, Config::Mqtt::PORT);
            Serial0.println("[mDNS] Service advertised: " + String(Config::Mqtt::MDNS_SERVICE) + " on port " + String(Config::Mqtt::PORT));
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

        // Handle mDNS queries
        if (mdnsStarted)
        {
            MDNS.update();
        }
    }
}
