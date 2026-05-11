#pragma once
#include <Arduino.h>

class IRfidReader {
public:
    virtual ~IRfidReader() = default;
    
    virtual bool init() = 0;
    virtual void setPower(uint8_t level) = 0;
    virtual bool isCardPresent() = 0;
    virtual String readUID() = 0;
    virtual void halt() = 0;
    virtual void softReset() = 0; 
};