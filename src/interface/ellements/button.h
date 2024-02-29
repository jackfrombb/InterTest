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

    // Настройки, которыми управляет кнопка
    ShareSetting *_setting = nullptr;

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

    /// @brief Получить настройку привязанную к кнопке или nullptr
    ShareSetting *getSetting()
    {
        return _setting;
    }

    /// @brief Установить настройку, которой будет управлять кнопка
    /// @param setting настройка
    ElTextButton *setShareSetting(ShareSetting *setting)
    {
        _setting = setting;

        //logi::p("Button", "set setting: " + String(setting->getArgs()->id) + " type: " + String(setting->getArgs()->settings_type));

        switch (setting->getArgs()->settings_type)
        {
        case share_setting_type::SETTING_TYPE_BOOL:

            setOnControlEvent([this](void *args, control_event_type event, ElementVirtual *el)
                              {
                                        ShareSetting *setting = (ShareSetting *)args;
                                        auto setArgs = (setting_args_bool *)setting->getArgs();

                                        // Может прийти только вкл и выкл, потому применяем
                                        // новое значение и сразу выходим из режима редактирования
                                        switch (event)
                                        {
                                        case control_event_type::PRESS_OK:
                                        {
                                            // Меняем значение
                                            setArgs->currentVal = !setArgs->currentVal;
                                            // Оповещаем
                                            setting->onChange();
                                            // Уведомляем об обработке нажатия
                                            return true;
                                        }
                                        }

                                        return false; },
                              setting);
            break;

        case share_setting_type::SETTING_TYPE_INT_RANGE:
        {
            // Переход в событие управления
            setOnControlEvent([this](void *args, control_event_type event, ElementVirtual *el)
                              { 
                                        if(event == control_event_type::PRESS_OK)
                                        {
                                            ElTextButton* button = (ElTextButton*)el;
                                            button->switchEditMode();
                                            return true;
                                        }
                                        return false; },
                              setting);

            // Устанавливаем управление режимом редактирования
            setOnEditModeEvent([this](void *arg)
                               {
                                        ShareSetting* setting = (ShareSetting* ) arg;
                                        auto args = (setting_args_int_range*) setting->getArgs();
                                        return args->currentVal; },
                               [this](int val, ElText *el, void *arg)
                               {
                                   ShareSetting *setting = (ShareSetting *)arg;
                                   auto args = (setting_args_int_range *)setting->getArgs();
                                   args->currentVal = range(val, args->fromVal, args->toVal);
                                   setting->onChange();
                                   return true;
                               },
                               setting);
            break;
        }

        case share_setting_type::SETTING_TYPE_INT_STEEP:
        {
            // Обработка события выбора. Перводим кнопку в режим редактирования
            setOnControlEvent([this](void *args, control_event_type event, ElementVirtual *el)
                              { 
                                        if(event == control_event_type::PRESS_OK)
                                        {
                                            ShareSetting* setting = (ShareSetting*) args;
                                            auto args = (setting_args_int_steep*) setting->getArgs();
                                            args->increaseCurrentVal();
                                            setting->onChange();
                                            return true;
                                        }
                                        return false; },
                              setting);
            break;
        }
        }

        return this;
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