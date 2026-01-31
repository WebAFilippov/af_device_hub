#pragma once
#include <EncButton.h>
#include "../core/DeviceState.h"
#include "../network/WiFiManager.h"

class Buttons
{
public:
    void begin();
    void update(DeviceState &state, WiFiManager &wifi);

private:
    Button up{10};
    Button down{11};
    Button setup{12};  // Setup button on pin 12

    bool upWasPressed = false;
    bool downWasPressed = false;
    unsigned long setupButtonPressTime = 0;
    bool setupButtonWasPressed = false;
    static constexpr unsigned long SETUP_HOLD_TIME = 5000; // 5 seconds
};

void Buttons::begin()
{
    pinMode(10, INPUT_PULLUP);
    pinMode(11, INPUT_PULLUP);
    pinMode(12, INPUT_PULLUP);  // Setup button
}

void Buttons::update(DeviceState &state, WiFiManager &wifi)
{
    up.tick();
    down.tick();
    setup.tick();

    // Motor control buttons - only control when pressed
    if (up.press())
        upWasPressed = true;
    if (down.press())
        downWasPressed = true;

    if (up.hold())
    {
        state.motorSpeed = 255;
    }
    else if (down.hold())
    {
        state.motorSpeed = -255;
    }
    else if (upWasPressed && up.release())
    {
        state.motorSpeed = 0;
        upWasPressed = false;
    }
    else if (downWasPressed && down.release())
    {
        state.motorSpeed = 0;
        downWasPressed = false;
    }

    // Setup button logic - 5 second hold to enable AP mode
    if (setup.press())
    {
        setupButtonPressTime = millis();
        setupButtonWasPressed = true;
        Serial0.println("[BTN] Setup button pressed, hold for 5 seconds...");
    }

    if (setupButtonWasPressed && setup.hold())
    {
        unsigned long holdDuration = millis() - setupButtonPressTime;
        if (holdDuration >= SETUP_HOLD_TIME)
        {
            Serial0.println("[BTN] Setup button held for 5 seconds, enabling AP mode");
            wifi.enableSetupMode();
            setupButtonWasPressed = false; // Reset to prevent multiple triggers
        }
    }

    if (setup.release())
    {
        if (setupButtonWasPressed)
        {
            unsigned long holdDuration = millis() - setupButtonPressTime;
            if (holdDuration < SETUP_HOLD_TIME)
            {
                Serial0.printf("[BTN] Setup button released after %lu ms (too short)\n", holdDuration);
            }
        }
        setupButtonWasPressed = false;
    }
}
