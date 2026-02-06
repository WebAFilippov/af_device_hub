#pragma once

#include <Arduino.h>

namespace Config
{
    // Firmware Version
    constexpr const char *FIRMWARE_VERSION = "1.0.0";
    constexpr const char *DEVICE_NAME = "AlexFil Hub";

    // Pin Definitions
    namespace Pins
    {
        // Motor (3-wire driver)
        constexpr uint8_t MOTOR_PWM = 4;
        constexpr uint8_t MOTOR_DIR = 5;
        constexpr uint8_t MOTOR_EN = 6;

        // Encoder (quadrature)
        constexpr uint8_t ENCODER_A = 15;
        constexpr uint8_t ENCODER_B = 16;

        // Buttons (moved to free up SPI pins for display)
        constexpr uint8_t BTN_UP = 17;
        constexpr uint8_t BTN_DOWN = 18;
        constexpr uint8_t BTN_SETUP = 19;
        
        // ADC (current sensor)
        constexpr uint8_t CURRENT_ADC = 7;
    }

    // Motor Settings
    namespace Motor
    {
        constexpr int MIN_DUTY = 70;
        constexpr int MAX_SPEED = 255;
        constexpr int SPEED_STEP = 10;
    }

    // Encoder Settings
    namespace Encoder
    {
        constexpr uint16_t FILTER_VALUE = 1023;
    }

    // Button Settings
    namespace Button
    {
        constexpr unsigned long SETUP_HOLD_TIME_MS = 5000;
        constexpr unsigned long DEBOUNCE_MS = 50;
    }

    // Current Sensor
    namespace Current
    {
        constexpr uint8_t ADC_RESOLUTION = 12;
        constexpr int ADC_THRESHOLD = 20;
        constexpr unsigned long READ_INTERVAL_MS = 100;
    }

    // WiFi Settings
    namespace WiFi
    {
        constexpr const char *AP_SSID = "AlexFil Developer";
        constexpr const char *AP_PASSWORD = ""; // Open network
        constexpr uint8_t AP_CHANNEL = 1;
        constexpr uint8_t AP_MAX_CONNECTIONS = 4;
        constexpr unsigned long CONNECT_TIMEOUT_MS = 15000;
        constexpr unsigned long RECONNECT_DELAY_MS = 5000;
        constexpr unsigned long SETUP_MODE_TIMEOUT_MS = 600000; // 10 minutes
    }

    // MQTT Settings
    namespace Mqtt
    {
        constexpr uint16_t PORT = 1883;
        constexpr unsigned long TELEMETRY_INTERVAL_MS = 1000;
        constexpr size_t MAX_MESSAGE_SIZE = 512;
        constexpr size_t MAX_TOPIC_SIZE = 64;

        // Topics
        constexpr const char *TOPIC_CMD_MOTOR = "hub/cmd/motor";
        constexpr const char *TOPIC_CMD_CONFIG = "hub/cmd/config";
        constexpr const char *TOPIC_TELEMETRY = "hub/telemetry";
        constexpr const char *TOPIC_STATUS = "hub/status";

        // mDNS
        constexpr const char *MDNS_HOSTNAME = "hub";
        constexpr const char *MDNS_SERVICE = "mqtt";
        constexpr const char *MDNS_PROTOCOL = "tcp";
    }

    // Web Server
    namespace Web
    {
        constexpr uint16_t PORT = 80;
        constexpr size_t MAX_REQUEST_SIZE = 1024;
        constexpr const char *API_PREFIX = "/api";
    }

    // Display Settings
    namespace Display
    {
        constexpr uint16_t WIDTH = 240;
        constexpr uint16_t HEIGHT = 320;
        constexpr uint8_t ROTATION = 0;                   // 0=портрет, 1=ландшафт
        constexpr unsigned long UPDATE_INTERVAL_MS = 250; // Обновление экрана каждые 250ms
    }

    // Serial/Debug
    namespace Debug
    {
        constexpr unsigned long BAUD_RATE = 115200;
        constexpr bool ENABLE_DEBUG_LOGS = true;

        // Log prefixes
        constexpr const char *LOG_WIFI = "[WiFi]";
        constexpr const char *LOG_MQTT = "[MQTT]";
        constexpr const char *LOG_MQTT_CTRL = "[MQTT_CTRL]";
        constexpr const char *LOG_WEB = "[WEB]";
        constexpr const char *LOG_BTN = "[BTN]";
        constexpr const char *LOG_ENCODER = "[ENC]";
        constexpr const char *LOG_CURRENT = "[CUR]";
        constexpr const char *LOG_MOTOR = "[MOTOR]";
        constexpr const char *LOG_DISPLAY = "[DISPLAY]";
    }

    // System
    namespace System
    {
        constexpr unsigned long WATCHDOG_TIMEOUT_MS = 30000;
        constexpr size_t TASK_STACK_SIZE = 4096;
        constexpr size_t EVENT_QUEUE_SIZE = 32;
    }
}
