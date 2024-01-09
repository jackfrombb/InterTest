#pragma once
#include "page_virtual.h"
#include "oscils/oscil_virtual.h"
#include "interface/ellements/ellements_list.h"

class OscilPage : public InterfacePageVirtual
{
private:
    OscilVirtual *_oscil;
    ElWaveform<uint16_t> _waveform;

public:
    OscilPage(MainBoard* mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _oscil = new OscilI2s(mainBoard, 10000);
        _oscil->init();
        _waveform.setPoints(_oscil->getBuffer(), _oscil->getBufferLength());

        _ellements = {
            &_waveform,
        };
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