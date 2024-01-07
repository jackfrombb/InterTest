#pragma once
#include <Arduino.h>
#include "interface/ellements/ellement_virtual.h"

class InterfacePageVirtual
{
protected:
    uint8_t _elCount;
private:
public:
    InterfacePageVirtual()
    {
    }

    virtual int8_t getSize()
    {
        return _elCount;
    }

    virtual EllementVirtual *getEllement(uint8_t num)
    {
        return nullptr;
    }
};