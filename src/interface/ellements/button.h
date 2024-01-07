#pragma once
#include <Arduino.h>
#include "ellement_virtual.h"

class ElButton : public EllementVirtual
{
private:
    String _title;
    bool _pushed;
    bool _selected;

public:
    ElButton()
    {
    }
    ElButton(String title, display_area area, el_size size) : EllementVirtual(size)
    {
        _title = title;
        _area = area;
    }
    ~ElButton()
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