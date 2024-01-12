#pragma once

#include <Arduino.h>
#include <vector>
#include "displays/display_virtual.h"
#include "interface/ellements/ellement_virtual.h"
#include "interface/pages/views/page_view.h"

class InterfacePageVirtual
{
protected:
    DisplayVirtual *_display;

private:
public:
    InterfacePageVirtual(DisplayVirtual *display) : _display(display)
    {
    }

    ~InterfacePageVirtual(){

    }

    virtual PageView* getPageView() = 0;

    virtual void onDraw(ulong onMillis){
        getPageView()->onDraw(onMillis);
    }

    virtual void onControlEvent(control_event_type eventType) {
        getPageView()->onControlEvent(eventType);
    }

};
