#pragma once
#include "displays/display_virtual.h"
#include "controls/control_virtual.h"
#include "interface/ellements/ellements_list.h"

/// @brief Визуальная часть страницы
class PageView : public ElGroup
{
protected:
    DisplayVirtual *_display;

public:
    PageView(DisplayVirtual *display)
    {
        _display = display;

        // На весь экран
        setArea(display_position{.leftUp = {
                                     .x = 0,
                                     .y = 0,
                                 },
                                 .rightDown{
                                     .x = display->getResolution().width,
                                     .y = display->getResolution().height,
                                 }});
    }

    virtual ~PageView()
    {
    }

    virtual bool onControlEvent(control_event_type eventType)
    {
        return false;
    }

    void onDraw() override
    {
    }

    virtual void onClose()
    {
    }
};