#pragma once
#include <EncButton.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"
#include "../network/WiFiManager.h"

class Buttons
{
public:
    void begin();
    void update(DeviceState &state, WiFiManager &wifi);

private:
    Button up{Config::Pins::BTN_UP};
    Button down{Config::Pins::BTN_DOWN};
    Button setup{Config::Pins::BTN_SETUP};

    bool upWasPressed = false;
    bool downWasPressed = false;
    unsigned long setupButtonPressTime = 0;
    bool setupButtonWasPressed = false;
};

void Buttons::begin()
{
    pinMode(Config::Pins::BTN_UP, INPUT_PULLUP);
    pinMode(Config::Pins::BTN_DOWN, INPUT_PULLUP);
    pinMode(Config::Pins::BTN_SETUP, INPUT_PULLUP);
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
        state.motorSpeed = Config::Motor::MAX_SPEED;
    }
    else if (down.hold())
    {
        state.motorSpeed = -Config::Motor::MAX_SPEED;
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

    // Setup button logic
    if (setup.press())
    {
        setupButtonPressTime = millis();
        setupButtonWasPressed = true;
        Serial0.println(Config::Debug::LOG_BTN " Setup button pressed, hold for " + String(Config::Button::SETUP_HOLD_TIME_MS / 1000) + " seconds...");
    }

    if (setupButtonWasPressed && setup.hold())
    {
        unsigned long holdDuration = millis() - setupButtonPressTime;
        if (holdDuration >= Config::Button::SETUP_HOLD_TIME_MS)
        {
            Serial0.println(Config::Debug::LOG_BTN " Setup button held, enabling AP mode");
            wifi.enableSetupMode();
            setupButtonWasPressed = false;
        }
    }

    if (setup.release())
    {
        if (setupButtonWasPressed)
        {
            unsigned long holdDuration = millis() - setupButtonPressTime;
            if (holdDuration < Config::Button::SETUP_HOLD_TIME_MS)
            {
                Serial0.printf(Config::Debug::LOG_BTN " Setup button released after %lu ms (too short)\n", holdDuration);
            }
        }
        setupButtonWasPressed = false;
    }
}
