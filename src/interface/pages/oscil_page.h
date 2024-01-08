#pragma once
#include "page_virtual.h"
#include "oscils/oscil_virtual.h"

class OscilPage : public InterfacePageVirtual
{
    private:
    OscilVirtual* _oscil;

    public:
    void setOscil(OscilVirtual* oscil){
        _oscil = oscil;
    }

};