#pragma once

#include <Arduino.h>
#include <vector>
#include "displays/display_virtual.h"
#include "interface/ellements/ellement_virtual.h"

class InterfacePageVirtual
{
protected:
    DisplayVirtual *_display = nullptr;
    std::vector<EllementVirtual *> _ellements;
    int8_t _count = 0;

private:
public:
    InterfacePageVirtual(DisplayVirtual *display) : _display(display)
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
