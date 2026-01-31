#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PicoMQTT.h>
#include "../core/DeviceState.h"

class MqttController
{
public:
    void begin(PicoMQTT::Server &broker);
    void update(DeviceState &state);

    // Process incoming MQTT messages
    void handleMessage(const char* topic, const char* payload);

    // Process commands (called by broker)
    void processMotorCommand(DeviceState &state, const char* payload);
    void processConfigCommand(DeviceState &state, const char* payload);

    // Publish message through broker
    void publish(const char* topic, const char* payload);

    // Topics
    static constexpr const char* TOPIC_CMD_MOTOR = "hub/cmd/motor";
    static constexpr const char* TOPIC_CMD_CONFIG = "hub/cmd/config";
    static constexpr const char* TOPIC_TELEMETRY = "hub/telemetry";
    static constexpr const char* TOPIC_STATUS = "hub/status";

private:
    PicoMQTT::Server* mqttBroker = nullptr;

    unsigned long lastTelemetryTime = 0;
    static constexpr unsigned long TELEMETRY_INTERVAL = 60;

    String pendingTopic;
    String pendingPayload;
    bool hasPendingPublish = false;

    void publishTelemetry(DeviceState &state);
};

void MqttController::begin(PicoMQTT::Server &broker)
{
    mqttBroker = &broker;

    Serial0.println("[MQTT_CTRL] Controller initialized");
}

void MqttController::update(DeviceState &state)
{
    // Publish telemetry periodically
    unsigned long now = millis();
    if (now - lastTelemetryTime >= TELEMETRY_INTERVAL)
    {
        lastTelemetryTime = now;
        publishTelemetry(state);
    }

    // Publish pending message
    if (hasPendingPublish && mqttBroker)
    {
        mqttBroker->publish(pendingTopic.c_str(), pendingPayload.c_str());
        hasPendingPublish = false;
    }
}

void MqttController::handleMessage(const char* topic, const char* payload)
{
    Serial0.printf("[MQTT_CTRL] Received: %s -> %s\n", topic, payload);
}

void MqttController::publish(const char* topic, const char* payload)
{
    if (mqttBroker)
    {
        pendingTopic = topic;
        pendingPayload = payload;
        hasPendingPublish = true;
    }
}

void MqttController::processMotorCommand(DeviceState &state, const char* payload)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial0.printf("[MQTT_CTRL] Failed to parse motor command: %s\n", error.c_str());
        return;
    }

    const char* action = doc["action"] | "stop";

    if (strcmp(action, "forward") == 0)
    {
        int speed = doc["speed"] | 255;
        state.motorSpeed = constrain(speed, 0, 255);
        Serial0.printf("[MQTT_CTRL] Motor forward: speed=%d\n", state.motorSpeed);
    }
    else if (strcmp(action, "backward") == 0)
    {
        int speed = doc["speed"] | 255;
        state.motorSpeed = constrain(-speed, -255, 0);
        Serial0.printf("[MQTT_CTRL] Motor backward: speed=%d\n", state.motorSpeed);
    }
    else if (strcmp(action, "stop") == 0)
    {
        state.motorSpeed = 0;
        Serial0.println("[MQTT_CTRL] Motor stop");
    }
    else if (strcmp(action, "set") == 0)
    {
        int speed = doc["speed"] | 0;
        state.motorSpeed = constrain(speed, -255, 255);
        Serial0.printf("[MQTT_CTRL] Motor set: speed=%d\n", state.motorSpeed);
    }
}

void MqttController::processConfigCommand(DeviceState &state, const char* payload)
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        Serial0.printf("[MQTT_CTRL] Failed to parse config: %s\n", error.c_str());
        return;
    }

    const char* param = doc["param"] | "";
    int value = doc["value"] | 0;

    Serial0.printf("[MQTT_CTRL] Config: %s = %d\n", param, value);

    if (strcmp(param, "speed") == 0)
    {
        state.motorSpeed = constrain(value, -255, 255);
        Serial0.printf("[MQTT_CTRL] Motor speed set to %d via config\n", state.motorSpeed);
    }
}

void MqttController::publishTelemetry(DeviceState &state)
{
    JsonDocument doc;
    doc["encoder"] = state.encoderPos;
    doc["current"] = state.currentAdc;
    doc["motorSpeed"] = state.motorSpeed;
    doc["wifiConnected"] = state.wifiConnected;

    char buffer[256];
    serializeJson(doc, buffer, sizeof(buffer));

    publish(TOPIC_TELEMETRY, buffer);
}
