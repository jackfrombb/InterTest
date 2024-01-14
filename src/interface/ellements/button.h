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
    uint8_t _buttonId;        // ID этой кнопки
    uint8_t *_selectedButton; // ID кнопки на которой сейчас курсор

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
    = default;

    ElTextButton *setEvent(ui_event event)
    {
        _event = event;
        return this;
    }

    ElTextButton *setButtonId(uint8_t buttonId)
    {
        _buttonId = buttonId;
        return this;
    }

    uint8_t getButtonId() const
    {
        return _buttonId;
    }

    ElTextButton *setSelectedButtonPtr(uint8_t *selectedButton)
    {
        _selectedButton = selectedButton;
        return this;
    }

    ui_event getEvent()
    {
        return _event;
    }

    ElTextButton *setPushed(bool pushed)
    {
        _pushed = pushed;
        return this;
    }

    bool isPushed()
    {
        return _pushed;
    }

    bool isSelected()
    {
        return _selectedButton != nullptr && *_selectedButton == _buttonId; //_selected;
    }

    el_type getElementType() override
    {
        return EL_TYPE_BUTTON;
    }
};