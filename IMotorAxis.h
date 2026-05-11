#pragma once
#include <Arduino.h>

class IMotorAxis {
public:
    virtual ~IMotorAxis() = default;
    
    virtual void init() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    
    virtual void setMaxSpeed(float speedHz) = 0;
    virtual void setAcceleration(float accel) = 0;
    
    virtual void moveTo(long absolutePosition) = 0;
    virtual void setCurrentPosition(long position) = 0;
    
    virtual long getCurrentPosition() = 0;
    
    virtual bool isMoving() = 0;
    virtual void stop() = 0;
};