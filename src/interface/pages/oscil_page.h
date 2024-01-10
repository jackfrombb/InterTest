#pragma once
#include "page_virtual.h"
#include "oscils/oscils_list.h"
#include "interface/ellements/ellements_list.h"

class OscilPage : public InterfacePageVirtual
{
private:
    OscilVirtual *_oscil;
    OscilPageView* _pageView;

public:
    OscilPage(MainBoard* mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new OscilPageView(mainBoard->getDisplay());
        _oscil = new OscilI2s(mainBoard, 1000);//new OscilAdcDma(mainBoard, 20000);// new OscilAdc(mainBoard, 5500); //new OscilI2s(mainBoard, 1000);
        
        initOscil();
    }

    ~OscilPage(){
        delete _oscil;
        delete _pageView;
    }

    void initOscil(){
        _oscil->init();
        _pageView->_waveform.setPoints(_oscil->getBuffer(), _oscil->getBufferLength());
        //_pageView->_waitText.setVisibility(false);
    }

    void setOscil(OscilVirtual *oscil)
    {
        if(_oscil != nullptr){
            _oscil->deinit();
            delete _oscil;
        }

        _oscil = oscil;
    }
    
    virtual PageView* getPageView(){
        return _pageView;
    }
};