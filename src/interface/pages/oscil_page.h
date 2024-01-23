#pragma once

#include "page_virtual.h"
#include "oscils/oscils_list.h"
#include "interface/ellements/ellements_list.h"
#include "voltmeter.h"
#include "views/wide_screen/oscil_page_view.h"

class OscilPage : public InterfacePageVirtual
{
private:
    OscilVirtual *_oscil;
    Voltmetr* _voltmeter;
    OscilPageView *_pageView;
    MainBoard *_mainBoard;
    uint16_t* voltBuffer;

public:
    explicit OscilPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {      
         _mainBoard = mainBoard;
        _oscil = new OscilI2s(mainBoard, 168000);// new OscilAdcDma(mainBoard, 20000);// new OscilAdc(mainBoard, 5500); //new OscilI2s(mainBoard, 1000);
        _voltmeter = new Voltmetr(_mainBoard);
        _voltmeter->setOscil(_oscil);
        _pageView = new OscilPageView(mainBoard->getDisplay(), _oscil, _voltmeter);
    
        initOscil();
    }

    ~OscilPage() override
    {
        logi::p("OscilPage", "Destroed");

        delete _oscil;
        delete _voltmeter;
        delete _pageView;
    }

    bool onControlEvent(control_event_type eventType) override
    {
        return InterfacePageVirtual::onControlEvent(eventType);
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