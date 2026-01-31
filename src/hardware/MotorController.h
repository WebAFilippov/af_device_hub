#pragma once
#include <GyverMotor2.h>
#include "../core/DeviceState.h"
#include "../core/Config.h"

class MotorController
{
public:
    void begin();
    void update(DeviceState &state);

private:
    GMotor2<DRIVER3WIRE> motor{Config::Pins::MOTOR_PWM, Config::Pins::MOTOR_EN, Config::Pins::MOTOR_DIR};
};

void MotorController::begin()
{
    motor.setMinDuty(Config::Motor::MIN_DUTY);
}

void MotorController::update(DeviceState &state)
{
    motor.setSpeed(state.motorSpeed);
}
