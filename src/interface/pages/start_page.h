#pragma once

#include "page_virtual.h"
#include "interface/pages/views/wide_screen/start_page_view.h" //TODO: Временное решение. Убрать перед добавлением SlimScreen

class StartPage : public InterfacePageVirtual
{
private:
    StartPageView *pageView;

public:
    StartPage(DisplayVirtual *display, function<void(pages_list)> onPageSelected) : InterfacePageVirtual(display)
    {
        pageView = new StartPageView(display, onPageSelected);
    }

    ~StartPage()
    {
        delete pageView;
        pageView = nullptr;
    }

    PageView *getPageView() override
    {
        return pageView;
    }

    bool onControlEvent(control_event_type eventType) override
    {
        pageView->onControlEvent(eventType);
        return true;
    }
};