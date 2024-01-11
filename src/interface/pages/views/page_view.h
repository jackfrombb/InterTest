#pragma once
#include "displays/display_virtual.h"
#include "interface/ellements/ellements_list.h"
#include "configuration.h"
#include <vector>

class PageView : public ElGroup
{
protected:
    DisplayVirtual *_display;

public:
    PageView(DisplayVirtual *display)
    {
        _display = display;
        
        //На весь экран
        setArea(display_position{.leftUp = {
                                 .x = 0,
                                 .y = 0,
                             },
                             .rightDown{
                                 .x = display->getResoluton().width,
                                 .y = display->getResoluton().height,
                             }});
    }
    ~PageView()
    {
    }
};