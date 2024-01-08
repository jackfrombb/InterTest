#pragma once

#include <Arduino.h>
#include <vector>
#include "displays/display_nokia_5110.h"
#include "interface/ellements/ellement_virtual.h"
#include "displays/display_virtual.h"

class InterfacePageVirtual
{
protected:
    std::vector<EllementVirtual *> _ellements;
    int8_t _count;
    DisplayVirtual _display;

private:
public:
    InterfacePageVirtual()
    {
    }

    virtual int8_t getSize()
    {
        return _count;
    }

    virtual EllementVirtual *getEllement(uint8_t num)
    {
        return _ellements[num];
    }

    virtual void addEllement(EllementVirtual *ellement)
    {
        _ellements.push_back(ellement);
    }
};
