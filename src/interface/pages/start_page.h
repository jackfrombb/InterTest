#pragma once
#include "page_virtual.h"
#include "interface/pages/views/wide_screen/start_page_view.h" //TODO: Временное решение. Убрать перед добавлением SlimScreen

class StartPage : public InterfacePageVirtual
{
private:
    StartPageView* pageView;
public:
    StartPage(DisplayVirtual *display) : InterfacePageVirtual(display)
    {
        
    }
    ~StartPage() = default;

    PageView *getPageView() override
    {
        return pageView;
    }
};