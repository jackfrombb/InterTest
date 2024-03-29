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
        return LOC_OSCIL; //"Осциллограф";//"Oscilloscope";
    case pages_list::PAGE_LIST_VOLT:
        return LOC_VOLTMETER; // "Вольтметр";//"Voltmeter";
    case pages_list::PAGE_LIST_OHM:
        return LOC_OHMMETER; // "Омметр";//"Ohmmeter";
    case pages_list::PAGE_GENERATOR:
        return LOC_GENERATOR; // "Генератор";//"Generator";
    case pages_list::PAGE_SETTINGS:
        return LOC_SETTING; //"Настройки";//"Settings";
    }

    return " "; // На доли секунды появляется на экране при быстрой прокрутке
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
