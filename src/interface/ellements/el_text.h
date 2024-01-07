#pragma once
#include "ellement_virtual.h"

class ElText : EllementVirtual
{
private:
    String *_text;

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
};