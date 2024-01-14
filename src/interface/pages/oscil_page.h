#pragma once

#include "page_virtual.h"
#include "oscils/oscils_list.h"
#include "interface/ellements/ellements_list.h"

class OscilPage : public InterfacePageVirtual
{
private:
    OscilVirtual *_oscil;
    OscilPageView *_pageView;
    MainBoard *_mainBoard;

public:
    explicit OscilPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _oscil = new OscilI2s(mainBoard, 98000); // new OscilAdcDma(mainBoard, 20000);// new OscilAdc(mainBoard, 5500); //new OscilI2s(mainBoard, 1000);
        _mainBoard = mainBoard;
        _pageView = new OscilPageView(mainBoard->getDisplay(), _oscil);

        initOscil();
    }

    ~OscilPage()
    {
        delete _oscil;
        delete _pageView;
    }

    void onControlEvent(control_event_type eventType) override
    {
        InterfacePageVirtual::onControlEvent(eventType);
    }

    void initOscil()
    {
        _oscil->init();
        _pageView->_waitText.setVisibility(false);
    }

    PageView *getPageView() override
    {
        return _pageView;
    }
};