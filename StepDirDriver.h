#pragma once
#include "IMotorAxis.h"
#include <FastAccelStepper.h>

extern FastAccelStepperEngine engine;

class StepDirDriver : public IMotorAxis {
private:
    FastAccelStepper* stepper;
    uint8_t stepPin, dirPin, enablePin;

public:
    StepDirDriver(uint8_t step, uint8_t dir, uint8_t en) 
        : stepPin(step), dirPin(dir), enablePin(en), stepper(nullptr) {}

    void init() override {
        pinMode(enablePin, OUTPUT);
        disable(); // Выключаем моторы до старта

        stepper = engine.stepperConnectToPin(stepPin);
        if (stepper) {
            stepper->setDirectionPin(dirPin);
            stepper->setAutoEnable(false); // Управляем Enable вручную 
        } else {
            Serial.println("SYS:ERROR: FastAccelStepper connect failed!");
        }
    }

    // Для DRV8825/A4988 LOW = включено
    void enable() override { digitalWrite(enablePin, LOW); }  
    void disable() override { digitalWrite(enablePin, HIGH); }

    void setMaxSpeed(float speedHz) override { if(stepper) stepper->setSpeedInHz(speedHz); }
    void setAcceleration(float accel) override { if(stepper) stepper->setAcceleration(accel); }

    void moveTo(long absolutePosition) override { if(stepper) stepper->moveTo(absolutePosition); }
    void setCurrentPosition(long position) override { if(stepper) stepper->setCurrentPosition(position); }
    
    long getCurrentPosition() override { return stepper ? stepper->getCurrentPosition() : 0; }

    bool isMoving() override { return stepper ? stepper->isRunning() : false; }
    void stop() override { if(stepper) stepper->stopMove(); }
};