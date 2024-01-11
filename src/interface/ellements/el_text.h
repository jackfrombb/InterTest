#pragma once
#include "ellement_virtual.h"

class ElText : public EllementVirtual
{
private:
    String *_text;
    el_text_size _elSize;

public:
    ElText()
    {
    }

    ElText(String *text)
    {
        _text = text;
    }

    ElText(String text)
    {
        _text = new String(text);
    }

    ~ElText()
    {
        if (_text != nullptr)
            delete _text;
    }

    void setText(String *text)
    {
        _text = text;
    }

    void setText(String text)
    {
        _text = new String(text);
    }

    String getText()
    {
        return *_text;
    }

    virtual void setPosition(int x, int y)
    {
        setArea(display_area{.leftUp{
            .x = x,
            .y = y,
        }});
    }

    virtual el_text_size getTextSize()
    {
        return _elSize;
    }

    virtual void setTextSize(el_text_size size)
    {
        _elSize = size;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_TEXT;
    }
};