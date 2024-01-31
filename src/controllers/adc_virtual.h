#pragma once

#include <Arduino.h>

class AdcVirtual
{
protected:
public:
    AdcVirtual()
    {
    }
    virtual ~AdcVirtual()
    {
    }

    virtual int8_t init() {}
    virtual int8_t deinit(){}
    virtual int8_t readData(uint16_t* buffer, uint16_t* readedLength) = 0;
};