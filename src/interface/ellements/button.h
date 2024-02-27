#pragma once
#include <Arduino.h>
#include "ellement_virtual.h"
#include "el_text.h"

class ElTextButton : public ElText
{
private:
    uint8_t _buttonId;        // ID этой кнопки
    uint8_t *_selectedButton; // ID кнопки на которой сейчас курсор

    bool _pushed;   // кнопка нажата
    bool _selected; // кнопка выбрана
    el_text_size _textSize;

    // Событие наведения фокуса на кнопку
    void *_onSelectedArgs = nullptr;
    function<void(void *args, ElementVirtual *el)> _onSelectedEvent = nullptr;

    // События контрола при наведении фокуса на кнопку
    void *_controlEventArgs = nullptr;
    function<bool(void *args, control_event_type event, ElementVirtual *el)> _onControlEvent = nullptr;

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

    ~ElTextButton() = default;

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

    ElTextButton *setOnControlEvent(function<bool(void *args, control_event_type event, ElementVirtual *el)> onControlEvent, void *args)
    {
        _onControlEvent = onControlEvent;
        _controlEventArgs = args;
        return this;
    }

    ElTextButton *setOnSelctedEvent(function<void(void *args, ElementVirtual *el)> onSelectedEvent)
    {
        _onSelectedEvent = onSelectedEvent;

        return this;
    }

    bool onControl(control_event_type type, void *args = nullptr) override
    {
        if (isInEditMode())
        {
            return ElText::onControl(type, args);
        }
        else
        {
            if (_onControlEvent != nullptr)
            {
                return _onControlEvent(_controlEventArgs, type, this);
            }
        }
        return false;
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