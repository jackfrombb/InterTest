#pragma once
#include <Arduino.h>
#include "ellement_virtual.h"

class ElTextButton : public EllementVirtual
{
private:
    String _title;
    bool _pushed;
    bool _selected;

public:
    ElTextButton()
    {
    }
    ElTextButton(String title, display_area area, el_size size) : EllementVirtual(size)
    {
        _title = title;
        _area = area;
    }
    ~ElTextButton()
    {
    }

    void setTitle(String title){
        _title = title;
    }

    String getTitle(){
        return _title;
    }

    void setPushed(bool pushed){
        _pushed = pushed;
    }

    bool isPushed(){
        return _pushed;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_BUTTON;
    }
};