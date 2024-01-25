#pragma once

#include "interface/pages/views/page_view.h"

class VoltemeterPageView : public PageView
{
private:
    ElText empty;

public:
    VoltemeterPageView(DisplayVirtual *display) : PageView(display)
    {
        empty.setText("В разработке")
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setArea({})
            ->setWidth(display->getWidth());

        addElement(&empty);
    }

    bool onControlEvent(control_event_type eventType) override
    {
        return false;
    }
};