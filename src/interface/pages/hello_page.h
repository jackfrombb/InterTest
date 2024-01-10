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
    HelloPage(DisplayVirtual *display) : InterfacePageVirtual(display), _progress(0.0)
    {
        _pageView = new HelloPageView(display, &_progress);
    }

    ~HelloPage()
    {
        Serial.println("Hello page: delted");
        delete _pageView;
    }

    virtual PageView* getPageView(){
        return _pageView;
    }

    float *getProgressPtr()
    {
        return &_progress;
    }
};