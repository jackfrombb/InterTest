#pragma once
#include <utility>

#include "ellement_virtual.h"

class ElText : public ElementVirtual
{
private:
    String _text;
    el_text_size _elSize = el_text_size::EL_TEXT_SIZE_MIDDLE;
    el_text_align _alignment = el_text_align::EL_TEXT_ALIGN_LEFT;
    std::function<String()> _calculateText = nullptr;

public:
    ElText()
    = default;

    explicit ElText(String text)
    {
        setText(std::move(text));
    }

    ~ElText()
    = default;

    ElText *setText(String text)
    {
        _text = std::move(text);
        return this;
    }

    ElText *setCalculatedText(std::function<String()> calculateText)
    {
        _calculateText = std::move(calculateText);
        return this;
    }

    String getText()
    {
        if (_calculateText != nullptr)
            return _calculateText();
        else
            return _text;
    }

    String *getTextPtr()
    {
        return &_text;
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

    virtual el_text_align getAlignment()
    {
        return _alignment;
    }

    virtual ElText *setAlignment(el_text_align alignment)
    {
        _alignment = alignment;
        return this;
    }

    el_type getElementType() override
    {
        return EL_TYPE_TEXT;
    }
};