#pragma once

class GeneratorPageView : public PageView
{
private:
    SignalGenerator *_generator;

    uint8_t _buttonFocus = 0; // Для управления переводом фокуса с кнопки на кнопку
    int8_t _buttonsCount = 0;

    ElScroll _elScroll = ElScroll(_display, false, false);

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
        uint16_t padding = 2;

        _elScroll
            .setX(0)
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight());

        _stateTitleText
            .setText(String(LOC_STATE) + ":") // "Состояние"
            ->setX(padding)
            ->setY(padding);

        _stateValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setCalculatedText([this]
                                {
                                    return _generator->isGenerationEnable() ? LOC_ON : LOC_OFF; // Если включен то надпись Вкл
                                })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(padding + _display->getMaxTextHeight(_stateTitleText.getTextSize()) + 10);

        _freqTitleText
            .setText(String(LOC_FREQ) + ":")
            ->setX(padding)
            ->setY(_stateValueButton.getY() + 20);

        _freqValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setCalculatedText([this]
                                { return String(_generator->getFrequensy()); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_freqTitleText.getY() + _display->getMaxTextHeight(_freqTitleText.getTextSize()) + 10);

        _dutyTitleText
            .setText(String(LOC_DUTY) + ":")
            ->setX(padding)
            ->setY(_freqValueButton.getY() + 20);

        _dutyValueButton
            .setButtonId(_buttonsCount++)
            ->setSelectedButtonPtr(&_buttonFocus)
            ->setCalculatedText([this]
                                { return String((uint8_t)(100.0 * _generator->getDutyCycle())); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_dutyTitleText.getY() + _display->getMaxTextHeight(_dutyTitleText.getTextSize()) + 10);

        _elScroll.addElement(&_stateTitleText)
            ->addElement(&_stateValueButton)
            ->addElement(&_freqTitleText)
            ->addElement(&_freqValueButton)
            ->addElement(&_dutyTitleText)
            ->addElement(&_dutyValueButton);
    }

    /// @brief Получить элемент до которого нужно прокрутить при переводе фокуса с кнопки на кнопку
    /// В данном случае это заголовок для настройки, которо й управляет кнопка
    /// @param id id кнопки на которую перевелся фокус
    ElementVirtual *_getTargetForId(uint8_t id)
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

        return &_stateValueButton;
    }

    void _onOkPress()
    {
        switch (_buttonFocus)
        {
        case 0:
            _generator->setEnable(!_generator->isGenerationEnable());
            break;
        case 1:
            break;
        case 2:
            break;
        }
    }

public:
    GeneratorPageView(DisplayVirtual *display, SignalGenerator *generator) : PageView(display)
    {
        _generator = generator;

        // Инициализация элементов
        _initElements();

        // Добавление элементов на страничку
        addElement(&_elScroll);
    }

    bool onControlEvent(control_event_type eventType) override
    {
        switch (eventType)
        {
        case control_event_type::PRESS_LEFT:
        {
            _buttonFocus = range(_buttonFocus - 1, 0, _buttonsCount - 1);
            _elScroll.smoothScrollTo(_getTargetForId(_buttonFocus));
            return true;
        }

        case control_event_type::PRESS_RIGHT:
        {
            _buttonFocus = range(_buttonFocus + 1, 0, _buttonsCount - 1);
            _elScroll.smoothScrollTo(_getTargetForId(_buttonFocus));
            return true;
        }

        case control_event_type::PRESS_OK:
            _onOkPress();
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