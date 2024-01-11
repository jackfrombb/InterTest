#pragma once
#include "ellement_virtual.h"

class ElText : public EllementVirtual
{
private:
    String *_text;
    el_text_size _elSize;
    el_text_align _alignment;

public:
    ElText()
    {
    }

    ElText(String *text)
    {
        setText(text);
    }

    ElText(String text)
    {
        setText(text);
    }

    ~ElText()
    {
        if (_text != nullptr)
            delete _text;
    }

    ElText *setText(String *text)
    {
        _text = text;
        return this;
    }

    ElText *setText(String text)
    {
        _text = new String(text);
        return this;
    }

    String getText()
    {
        return *_text;
    }

    virtual ElText *setPosition(int x, int y)
    {
        setArea(display_position{.leftUp{
            .x = x,
            .y = y,
        }});

        return this;
    }

    virtual el_text_size getTextSize()
    {
        return _elSize;
    }

    virtual ElText *setTextSize(el_text_size size)
    {
        _elSize = size;
        return this;
    }

    virtual el_text_align getAlignment(){
        return _alignment;
    }

    virtual ElText* setAlignment(el_text_align alignment){
        _alignment = alignment;
        return this;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_TEXT;
    }
};