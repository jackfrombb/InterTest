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
    OscilPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new OscilPageView(mainBoard->getDisplay());
        _oscil = new OscilI2s(mainBoard, 1000); // new OscilAdcDma(mainBoard, 20000);// new OscilAdc(mainBoard, 5500); //new OscilI2s(mainBoard, 1000);
        _mainBoard = mainBoard;
        
        // Подключаем упарвление
        _mainBoard->getControl()->attachControlHandler(controlEvent, this);

        initOscil();
    }

    ~OscilPage()
    {
        delete _oscil;
        delete _pageView;
    }

    static void controlEvent(control_event_type eventType, void *args)
    {
        OscilPage *page = (OscilPage *)args;

        switch (eventType)
        {
        case control_event_type::PRESS_OK:
            Serial.println("Press OK");
            break;

        case control_event_type::PRESS_LEFT:
        {
            Serial.println("Press LEFT" + String(page->steepChngeOscilMeasures(false, 1)));
            break;
        }

        case control_event_type::PRESS_RIGHT:
            Serial.println("Press RIGHT" + String(page->steepChngeOscilMeasures(true, 1)));
            break;

        case control_event_type::LONG_PRESS_OK:
            Serial.println("Press LONG OK");
            break;

        case control_event_type::LONG_PRESS_RIGHT:
            Serial.println("Press LONG RIGHT"+ String(page->steepChngeOscilMeasures(true, 500)));
            break;

        case control_event_type::LONG_PRESS_LEFT:
            Serial.println("Press LONG LEFT"+ String(page->steepChngeOscilMeasures(false, 500)));
            break;
        }
    }

    void initOscil()
    {
        _oscil->init();
        _pageView->_waveform.setPoints(_oscil->getBuffer(), _oscil->getBufferLength());
        _pageView->_waitText.setVisibility(false);
    }

    uint32_t steepChngeOscilMeasures(bool increase, int16_t multipler){

        auto t = _oscil->getMeasuresInSecond();
        t = increase ? t + (1*multipler) : t - (1*multipler);
        _oscil->setMeasuresInSecond(t);

        return t;
    }

    OscilVirtual *getOscil()
    {
        return _oscil;
    }

    void setOscil(OscilVirtual *oscil)
    {
        if (_oscil != nullptr)
        {
            _oscil->deinit();
            delete _oscil;
        }

        _oscil = oscil;
    }

    virtual PageView *getPageView()
    {
        return _pageView;
    }
};