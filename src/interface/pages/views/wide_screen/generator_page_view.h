#pragma once

class GeneratorPageView : public PageView
{
private:
    SignalGenerator *_generator;

    uint8_t _buttonFocus = 0; // Для управления переводом фокуса с кнопки на кнопку

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

        _stateTitleText
            .setText(String(LOC_STATE) + ":") // "Состояние"
            ->setX(padding)
            ->setY(padding);

        _stateValueButton
            .setButtonId(0)
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
            .setButtonId(1)
            ->setCalculatedText([this]
                                { return String(_generator->getFrequensy()); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_freqTitleText.getY() + _display->getMaxTextHeight(_freqTitleText.getTextSize()) + 10);

        _dutyTitleText
            .setText(String(LOC_DUTY) + ":")
            ->setX(padding)
            ->setY(_freqValueButton.getY() + 20);

        _dutyValueButton
            .setButtonId(2)
            ->setCalculatedText([this]
                                { return String((uint8_t)(100.0 * _generator->getDutyCycle())); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(_dutyTitleText.getY() + _display->getMaxTextHeight(_dutyTitleText.getTextSize()) + 10);
    }

public:
    GeneratorPageView(DisplayVirtual *display, SignalGenerator *generator) : PageView(display)
    {
        _generator = generator;

        // Инициализация элементов
        _initElements();

        // Добавление элементов на страничку
        addElement(&_stateTitleText)
            ->addElement(&_stateValueButton)
            ->addElement(&_freqTitleText)
            ->addElement(&_freqValueButton)
            ->addElement(&_dutyTitleText)
            ->addElement(&_dutyValueButton);
    }

    bool *getIsGeneratorEnabledPtr()
    {
        return &_isGeneratorEnabled;
    }

    String *getGenerationFreqPtr()
    {
        return _freqValuePtr;
    }

    String *getDutyCyclePtr()
    {
        return _dutyValuePtr;
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