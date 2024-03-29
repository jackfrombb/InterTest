#pragma once

class GeneratorPageView : public PageView
{
private:
    PwmControllerVirtual *_generator;

    uint8_t _buttonFocus = 0; // Для управления переводом фокуса с кнопки на кнопку
    int8_t _buttonsCount = 0; // Для назначения номера кнопки и хранит кол-во кнопок
    bool _inEditMode = false; // Флаг входа в редактирование настройки

    ElScroll _elScroll = ElScroll(false, false);
    ElScrollBar _elScrollBar = ElScrollBar(true);

    ElText _stateTitleText;         // Состояние генератора. Надпись
    ElTextButton _stateValueButton; // Переключение состояния генератора
    bool _isGeneratorEnabled;       // Состояние генератора

    ElText _freqTitleText;         // Частота сигнала. Надпись
    ElTextButton _freqValueButton; // Изменение частоты генерации
    String *_freqValuePtr;         // Указатель на частоту

    ElText _dutyTitleText;         // Скважность сигнала. Надпись
    ElTextButton _dutyValueButton; // Изменение скважности
    String *_dutyValuePtr;         // Указатель на значение скважности

    void _initElements()
    {
        // Для удобсва указания отступов
        uint16_t padding = 2;

        // Инициализируем корневую группу для прокрутки
        _elScroll
            .setX(0)
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight());

        // Состояние генератора надпись
        _stateTitleText
            .setText(String(LOC_STATE) + ":") // "Состояние"
            ->setX(padding)
            ->setY(padding);

        // Состояние генератора кнопка
        _stateValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setShareSetting(_generator->getStateSetting())
            ->setCalculatedText([this](void *arg)
                                {
                                    return _generator->isEnable() ? LOC_ON : LOC_OFF; // Если включен то надпись Вкл
                                })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(padding + _display->getMaxTextHeight(_stateTitleText.getTextSize()) + 10);

        // Частота генератора надпись
        _freqTitleText
            .setText(String(LOC_FREQ) + ":")
            ->setX(padding)
            ->setY(_stateValueButton.getY() + 20);

        // Частота кнопка
        _freqValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setShareSetting(_generator->getFreqSetting())
            ->setCalculatedText([this](void *arg)
                                { 
                                    ShareSetting* setting = (ShareSetting*) arg;
                                    setting_args_int_range* settingArgs = (setting_args_int_range*) setting->getArgs();
                                    return String(settingArgs->currentVal); },
                                _generator->getFreqSetting())
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_freqTitleText.getY() + _display->getMaxTextHeight(_freqTitleText.getTextSize()) + 10);

        // Скважность генератора надпись
        _dutyTitleText
            .setText(String(LOC_DUTY) + ":")
            ->setX(padding)
            ->setY(_freqValueButton.getY() + 20);

        // Скважность кнопка
        _dutyValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setShareSetting(_generator->getDutySetting())
            ->setCalculatedText([this](void *arg)
                                { 
                                    ShareSetting* setting = (ShareSetting*) arg;
                                    setting_args_int_steep* settingArgs = (setting_args_int_steep*) setting->getArgs();
                                    return String(settingArgs->getSteepValue()); },
                                _generator->getDutySetting())
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_dutyTitleText.getY() + _display->getMaxTextHeight(_dutyTitleText.getTextSize()) + 10);

        // Добавляем элементы в прокрутку
        _elScroll.addElement(&_stateTitleText)
            ->addElement(&_stateValueButton)
            ->addElement(&_freqTitleText)
            ->addElement(&_freqValueButton)
            ->addElement(&_dutyTitleText)
            ->addElement(&_dutyValueButton);

        // Инициализируем индикацию прокрутки
        _elScrollBar.setArea({.leftUp = {.x = (int)_display->getWidth() - 6, .y = (int)5},
                              .rightDown{.x = (int)_display->getWidth() - 2, .y = (int)_display->getHeight() - 5}});
        // Вычисляем положение прокрутки
        _calculateScrollBarPosition();
    }

    void _calculateScrollBarPosition()
    {
        uint16_t height = _dutyValueButton.getY() + 10;
        uint16_t pos = _getTargetForId(_buttonFocus)->getY();
        _elScrollBar.setScrollPosition(
            height,
            (height / (_buttonsCount - 1)) * _buttonFocus,
            (uint16_t)(height / (_buttonsCount - 1)));
    }

    /// @brief Получить элемент до которого нужно прокрутить при переводе фокуса с кнопки на кнопку
    /// В данном случае это заголовок для настройки, которо й управляет кнопка
    /// @param id id кнопки на которую перевелся фокус
    ElementVirtual *_getTargetForId(uint8_t id)
    {
        try
        {
            switch (id)
            {
            case 0:
                return &_stateTitleText;

            case 1:
                return &_freqTitleText;

            case 2:
                return &_dutyTitleText;
            }

            return nullptr;
        }
        catch (exception ex)
        {
            return nullptr;
        }
    }

    /// @brief Первести фокус
    /// @param dir направление первода фокуса (-1 назад, +1 вперед)
    void _changeButtonFocus(int8_t dir)
    {
        _buttonFocus = range(_buttonFocus + (1 * dir), 0, _buttonsCount - 1);
        _elScroll.smoothScrollTo(_getTargetForId(_buttonFocus));
        _calculateScrollBarPosition();
    }

    bool _sendControlEventToButton(control_event_type event)
    {
        bool isEventProcessed = false;
        switch (_buttonFocus)
        {
        case 0:
            isEventProcessed = _stateValueButton.onControl(event);
            _inEditMode = _stateValueButton.isInEditMode();
            break;
        case 1:
            isEventProcessed = _freqValueButton.onControl(event);
            _inEditMode = _freqValueButton.isInEditMode();
            break;
        case 2:
            isEventProcessed = _dutyValueButton.onControl(event);
            _inEditMode = _dutyValueButton.isInEditMode();
            break;
        }

        return isEventProcessed;
    }

public:
    GeneratorPageView(DisplayVirtual *display, PwmControllerVirtual *generator) : PageView(display)
    {
        _generator = generator;

        // Инициализация элементов
        _initElements();

        // Добавление элементов на страничку
        addElement(&_elScroll)->addElement(&_elScrollBar);
    }

    bool onControlEvent(control_event_type eventType) override
    {
        if (_inEditMode)
        {
            return _sendControlEventToButton(eventType);
        }
        else
            switch (eventType)
            {
            case control_event_type::PRESS_LEFT:
            {
                _changeButtonFocus(-1);
                return true;
            }

            case control_event_type::PRESS_RIGHT:
            {
                _changeButtonFocus(1);
                return true;
            }

            // При нажатии на ок или вращении энкодера с зажатой кнопкой отправляем событие в текущую кнопку
            case control_event_type::LONG_PRESS_LEFT:
            case control_event_type::LONG_PRESS_RIGHT:
            case control_event_type::PRESS_OK:
                //_onOkPress();
                _sendControlEventToButton(eventType);
                return true;
            }

        return false;
    }

    void onDraw() override
    {
    }
};

/*
Заметки:
На странице нужно:
- Включать и выключать генерацию
- Управлять частотой и скважностью сигнала
В будущем можно будет изменять тип сигнала, но для начала достаточно меандра

Состояние: Вкл. Выкл
Частота: 2000 hz (Подчеркнуто при вводе)
Скважность: 50% (меняется при )
*/