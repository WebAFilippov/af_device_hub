#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"

class WiFiManager
{
public:
    void begin(DeviceState &state);
    void update(DeviceState &state);
    void enableSetupMode();
    bool isInSetupMode() { return setupModeActive; }

private:
    Preferences prefs;
    bool setupModeActive = false;
    bool apEnabled = false;
    unsigned long setupModeStartTime = 0;
    unsigned long lastActivityTime = 0;

    // Reconnection logic
    unsigned long connectStartTime = 0;
    unsigned long lastReconnectAttempt = 0;
    bool isReconnectPending = false;

    void startAP();
    void stopAP();
    void connectSTA(const String &ssid, const String &pass);
    void checkActivityTimeout();
    void handleReconnect(const String &ssid, const String &pass);
};

void WiFiManager::begin(DeviceState &state)
{
    prefs.begin("wifi-cfg", false);

    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");
    state.savedSsid = ssid;

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    connectSTA(ssid, pass);
}

void WiFiManager::update(DeviceState &state)
{
    wl_status_t st = WiFi.status();
    state.wifiConnected = (st == WL_CONNECTED);

    if (!setupModeActive)
    {
        // Normal mode - handle connection and reconnection
        if (st == WL_CONNECTED)
        {
            // Connected - reset all reconnection state
            connectStartTime = 0;
            isReconnectPending = false;
        }
        else if (!state.savedSsid.isEmpty())
        {
            // Not connected but have saved credentials
            if (isReconnectPending)
            {
                // Waiting to retry - check if delay passed
                if (millis() - lastReconnectAttempt >= Config::WiFi::RECONNECT_DELAY_MS)
                {
                    Serial0.println(Config::Debug::LOG_WIFI " Retrying connection...");
                    isReconnectPending = false;
                    connectStartTime = millis();
                    connectSTA(state.savedSsid, prefs.getString("pass", ""));
                }
            }
            else if (connectStartTime == 0)
            {
                // Not trying yet - start now
                connectStartTime = millis();
                connectSTA(state.savedSsid, prefs.getString("pass", ""));
            }
            else if (millis() - connectStartTime > Config::WiFi::CONNECT_TIMEOUT_MS)
            {
                // Timeout - disconnect and schedule retry
                Serial0.println(Config::Debug::LOG_WIFI " Connect timeout. Will retry in " + String(Config::WiFi::RECONNECT_DELAY_MS / 1000) + " seconds...");
                WiFi.disconnect();
                isReconnectPending = true;
                lastReconnectAttempt = millis();
                connectStartTime = 0;
            }
        }
    }
    else
    {
        // Setup mode - check for timeout
        checkActivityTimeout();
    }
}

void WiFiManager::enableSetupMode()
{
    if (!setupModeActive)
    {
        setupModeActive = true;
        setupModeStartTime = millis();
        lastActivityTime = millis();
        startAP();
        Serial0.println(Config::Debug::LOG_WIFI " Setup mode enabled (AP active for " + String(Config::WiFi::SETUP_MODE_TIMEOUT_MS / 60000) + " minutes)");
    }
    else
    {
        // Reset activity timer on button press
        lastActivityTime = millis();
        Serial0.println(Config::Debug::LOG_WIFI " Activity detected, resetting timeout");
    }
}

void WiFiManager::checkActivityTimeout()
{
    if (setupModeActive && millis() - lastActivityTime > Config::WiFi::SETUP_MODE_TIMEOUT_MS)
    {
        Serial0.println(Config::Debug::LOG_WIFI " Setup mode timeout - disabling AP");
        stopAP();
        setupModeActive = false;

        // Try to reconnect to saved WiFi if exists
        String ssid = prefs.getString("ssid", "");
        String pass = prefs.getString("pass", "");
        if (!ssid.isEmpty())
        {
            connectSTA(ssid, pass);
        }
    }
}

void WiFiManager::startAP()
{
    WiFi.mode(WIFI_AP_STA);
    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

    if (WiFi.softAP(Config::WiFi::AP_SSID, Config::WiFi::AP_PASSWORD, Config::WiFi::AP_CHANNEL, 0, Config::WiFi::AP_MAX_CONNECTIONS))
    {
        apEnabled = true;
        Serial0.println(Config::Debug::LOG_WIFI " AP started: 192.168.4.1");
    }
}

void WiFiManager::stopAP()
{
    WiFi.softAPdisconnect(true);
    apEnabled = false;
    Serial0.println(Config::Debug::LOG_WIFI " AP stopped");

    // Switch back to STA mode only
    WiFi.mode(WIFI_STA);
}

void WiFiManager::connectSTA(const String &ssid, const String &pass)
{
    if (ssid.isEmpty())
        return;

    Serial0.print(Config::Debug::LOG_WIFI " Connecting to ");
    Serial0.println(ssid);

    WiFi.disconnect();

    WiFi.begin(ssid.c_str(), pass.c_str());
}
