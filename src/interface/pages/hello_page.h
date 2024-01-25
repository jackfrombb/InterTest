#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"
#include "displays/display_helper.h"
#include "displays/display_virtual.h"


class HelloPage : public InterfacePageVirtual
{
private:
    float _progress;
    HelloPageView* _pageView;

public:
    explicit HelloPage(DisplayVirtual *display) : InterfacePageVirtual(display), _progress(0.0)
    {
        _pageView = new HelloPageView(display, &_progress);
    }

    ~HelloPage()
    {
        delete _pageView;
        _pageView = nullptr;
    }

    PageView* getPageView() override{
        return _pageView;
    }

    float *getProgressPtr()
    {
        return &_progress;
    }
};