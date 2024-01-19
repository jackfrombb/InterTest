#pragma once

#include <Arduino.h>
#include <vector>
#include "displays/display_virtual.h"
#include "interface/ellements/ellement_virtual.h"
#include "interface/pages/views/page_view.h"
#include "controls/control_virtual.h"

typedef enum {
    PAGE_LIST_OSCIL, PAGE_LIST_VOLT, PAGE_LIST_OHM
} pages_list;

/// @brief Контроллер для страницы. Управление основными объектами страницы здесь
class InterfacePageVirtual
{
protected:
    DisplayVirtual *_display;

private:
public:
    explicit InterfacePageVirtual(DisplayVirtual *display) : _display(display) {}

    ~InterfacePageVirtual() = default;

    virtual PageView *getPageView() = 0;

    virtual void onControlEvent(control_event_type eventType)
    {
        getPageView()->onControlEvent(eventType);
    }
};
