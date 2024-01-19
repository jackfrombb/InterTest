#pragma once

#include "interface/pages/views/page_view.h"
#include "interface/pages/page_virtual.h"

class StartPageView : public PageView
{
    private:
    ElText _prev;
    ElText _current;
    ElText _next;
    void(*_onPageSelected)(pages_list page);

    public:
    StartPageView(DisplayVirtual* display, void(*onPageSelected)(pages_list page)) : PageView(display)
    {
        _onPageSelected = onPageSelected;
    }

    ~StartPageView() = default;


};