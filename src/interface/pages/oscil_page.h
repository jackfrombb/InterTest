#pragma once
#include "page_virtual.h"
#include "oscils/oscil_virtual.h"
#include "interface/ellements/ellements_list.h"

class OscilPage : public InterfacePageVirtual
{
private:
    OscilVirtual *_oscil;
    ElWaveform<uint16_t> _waveform;

    void _initWaveform(){
        display_area size = {
            .leftUp{.x =0, .y =0, },
            .rightDown {.x = _display->getResoluton().width, .y = _display->getResoluton().height}
        };
        _waveform.setArea(size);
    }

public:
    OscilPage(MainBoard* mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _oscil = new OscilI2s(mainBoard, 1000);
        initOscil();
        _initWaveform();

        _ellements = {
            &_waveform,
        };
    }

    void initOscil(){
        _oscil->init();
        _waveform.setPoints(_oscil->getBuffer(), _oscil->getBufferLength());
    }

    void setOscil(OscilVirtual *oscil)
    {
        if(_oscil != nullptr){
            _oscil->deinit();
            delete _oscil;
        }

        _oscil = oscil;
    }
};