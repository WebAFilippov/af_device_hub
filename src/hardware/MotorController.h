#pragma once
#include <GyverMotor2.h>
#include "../core/DeviceState.h"

class MotorController
{
public:
    void begin();
    void update(DeviceState &state);

private:
    GMotor2<DRIVER3WIRE> motor{4, 6, 5};
};

void MotorController::begin()
{
    motor.setMinDuty(70);
}

void MotorController::update(DeviceState &state)
{
    motor.setSpeed(state.motorSpeed);
}
