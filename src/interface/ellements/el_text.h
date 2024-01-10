#pragma once
#include "ellement_virtual.h"

class ElText : public EllementVirtual
{
private:
    String *_text;
    el_text_align _textAlignment;
    int16_t _elGroup;

public:
    ElText()
    {
        _elGroup = -1;
        _textAlignment = EL_TEXT_ALIGN_LEFT;
    }

    ElText(String *text)
    {
        _text = text;
        _elGroup = -1;
        _textAlignment = EL_TEXT_ALIGN_LEFT;
    }

    ElText(String text)
    {
        _text = new String(text);
        _textAlignment = EL_TEXT_ALIGN_LEFT;
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

    virtual el_type getEllementType()
    {
        return EL_TYPE_TEXT;
    }

    el_text_align getTextAlignment()
    {
        return _textAlignment;
    }

    void setTextAlignment(el_text_align align)
    {
        _textAlignment = align;
    }
};