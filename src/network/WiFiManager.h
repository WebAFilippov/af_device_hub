#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include "../core/DeviceState.h"

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
    static constexpr unsigned long SETUP_TIMEOUT = 600000;    // 10 minutes
    static constexpr unsigned long ACTIVITY_TIMEOUT = 600000; // 10 minutes

    // Reconnection logic
    unsigned long connectStartTime = 0;
    unsigned long lastReconnectAttempt = 0;
    bool isReconnectPending = false;
    static constexpr unsigned long CONNECT_TIMEOUT = 15000;     // 15 seconds to try connecting
    static constexpr unsigned long RECONNECT_DELAY = 5000;      // 5 seconds between retries

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
                if (millis() - lastReconnectAttempt >= RECONNECT_DELAY)
                {
                    Serial0.println("[WiFi] Retrying connection...");
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
            else if (millis() - connectStartTime > CONNECT_TIMEOUT)
            {
                // Timeout - disconnect and schedule retry
                Serial0.println("[WiFi] Connect timeout. Will retry in 5 seconds...");
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
        Serial0.println("[WiFi] Setup mode enabled (AP active for 10 minutes)");
    }
    else
    {
        // Reset activity timer on button press
        lastActivityTime = millis();
        Serial0.println("[WiFi] Activity detected, resetting timeout");
    }
}

void WiFiManager::checkActivityTimeout()
{
    if (setupModeActive && millis() - lastActivityTime > ACTIVITY_TIMEOUT)
    {
        Serial0.println("[WiFi] Setup mode timeout - disabling AP");
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

    if (WiFi.softAP("AlexFil Developer", ""))
    {
        apEnabled = true;
        Serial0.println("[WiFi] AP started: 192.168.4.1");
    }
}

void WiFiManager::stopAP()
{
    WiFi.softAPdisconnect(true);
    apEnabled = false;
    Serial0.println("[WiFi] AP stopped");

    // Switch back to STA mode only
    WiFi.mode(WIFI_STA);
}

void WiFiManager::connectSTA(const String &ssid, const String &pass)
{
    if (ssid.isEmpty())
        return;

    Serial0.print("[WiFi] Connecting to ");
    Serial0.println(ssid);

    WiFi.disconnect();

    WiFi.begin(ssid.c_str(), pass.c_str());
}
