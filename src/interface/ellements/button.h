#pragma once
#include <Arduino.h>
#include "ellement_virtual.h"
#include "el_text.h"

typedef struct
{
    void (*event)(void *args);
    void *args;
} ui_event;

class ElTextButton : public ElText
{
private:
    bool _pushed;    // кнопка нажата
    bool _selected;  // кнопка выбрана
    ui_event _event; // событие по нажатию кнопки
    el_text_size _textSize;

public:
    ElTextButton()
    {
        setArea({});
        _selected = false;
        _pushed = false;
    }

    ElTextButton(String title, display_position area, el_text_size size) : ElText(title)
    {
        setArea(area);
        _selected = false;
        _pushed = false;
    }
    ~ElTextButton()
    {
    }

    ElTextButton *setEvent(ui_event event)
    {
        _event = event;
        return this;
    }

    ui_event getEvent()
    {
        return _event;
    }

    ElTextButton *setPushed(bool pushed)
    {
        _pushed = pushed;
        if (_pushed)
        {
            _event.event(_event.args);
        }

        return this;
    }

    bool isPushed()
    {
        return _pushed;
    }

    ElTextButton *setSelected(bool selected)
    {
        _selected = selected;
        return this;
    }

    bool isSelected()
    {
        return _selected;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_BUTTON;
    }
};