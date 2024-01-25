#pragma once

#include "page_virtual.h"
#include "interface/pages/views/wide_screen/voltmeter_page_view.h"

class VoltmeterPage : public InterfacePageVirtual
{
private:
    VoltemeterPageView *_pageView;

public:
    VoltmeterPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new VoltemeterPageView(mainBoard->getDisplay());
    }

    ~VoltmeterPage()
    {
        delete _pageView;
        _pageView = nullptr;
    }

    PageView *getPageView() override
    {
        return _pageView;
    }

    bool onControlEvent(control_event_type eventType) override
    {
        return _pageView->onControlEvent(eventType);
    }
};