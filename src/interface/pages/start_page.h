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
    }

    PageView *getPageView() override
    {
        return pageView;
    }
};