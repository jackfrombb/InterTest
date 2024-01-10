#pragma once
#include "displays/display_virtual.h"
#include "interface/ellements/ellements_list.h"
#include "configuration.h"
#include <vector>

class PageView
{
protected:
    DisplayVirtual *_display;
    std::vector<EllementVirtual *> _ellements;

public:
    PageView(DisplayVirtual *display)
    {
        _display = display;
    }
    ~PageView()
    {
    }

    virtual int8_t getEllementsSize()
    {
        return _ellements.size();
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