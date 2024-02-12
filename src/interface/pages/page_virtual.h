#pragma once

#include <Arduino.h>
#include <vector>
// #include "displays/display_virtual.h"
// #include "interface/ellements/ellement_virtual.h"
// #include "interface/pages/views/page_view.h"
// #include "controls/control_virtual.h"

typedef enum
{
    PAGE_LIST_OSCIL,
    PAGE_LIST_VOLT,
    PAGE_LIST_OHM,
    PAGE_GENERATOR,
    PAGE_SETTINGS,
    pagesCount, // Для определения кол-ва страниц
} pages_list;

String pages_list_getName(pages_list page)
{
    switch (page)
    {
    case pages_list::PAGE_LIST_OSCIL:
        return "Осциллограф";
    case pages_list::PAGE_LIST_VOLT:
        return "Вольтметр";
    case pages_list::PAGE_LIST_OHM:
        return "Омметр";
    case pages_list::PAGE_GENERATOR:
        return "Генератор";
    case pages_list::PAGE_SETTINGS:
        return "Настройки";
    }

    return "Error_02";
}

/// @brief Контроллер для страницы. Управление основными объектами страницы здесь
class InterfacePageVirtual
{
protected:
    DisplayVirtual *_display;

private:
public:
    explicit InterfacePageVirtual(DisplayVirtual *display) : _display(display) {}

    virtual ~InterfacePageVirtual() = default;


    virtual bool onControlEvent(control_event_type eventType)
    {
        return getPageView()->onControlEvent(eventType);
    }

    virtual void onClose()
    {
        getPageView()->onClose();
    }

    virtual void onDraw()
    {
    }
    
    virtual PageView *getPageView() = 0;
};
