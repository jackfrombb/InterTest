#pragma once
#include "interface/pages/views/page_view.h"
#include "voltmeter.h"
#include <math.h>

using namespace std;

class OscilPageView : public PageView
{
private:
    ElCenteredGroup _bottomButtons;
    OscilVirtual *_oscil;
    Voltmetr *_voltmeter;
    InterfaceEngineVirtual *_iEngine;

    bool _isOnSampleChangeMod = false;
    int16_t _editSmpsPosition = -1;
    // uint16_t _smapleChangeMultipler = 1;

    uint8_t _selectedMeasuresMode = 0; // 0 - Пик ту пик, 1 - среднее, 2 - герцы

    int _defaultBottomMenuPosition; // Изначальное, нормальное положение меню

    ulong _lastButtonPressTime;

    void _initInfoTexts()
    {
        oscilFreqText.setCalculatedText([this]()
                                        { return String(_oscil->getMeasuresInSecond()); })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setX(0)
            ->setY(_defaultBottomMenuPosition)
            ->setWidth(_display->getWidth())
            ->setVisibility(false);
    }

    void _initBottomMenu()
    {
        _volt.setButtonId(0)
            ->setSelectedButtonPtr(&selectedButton)
            ->setCalculatedText(
                [this]
                {
                String out = "";
                switch(_selectedMeasuresMode){
                    case 0:
                    //Максимальное значение напряжения
                    out = String(_voltmeter->getMax()) + "v";
                    break;

                    case 1:
                    //Среднее значение напряжения
                    out = String(_voltmeter->getMiddle()) + "v";
                    break;

                    case 2:
                    //Когда нибудь будем тут частоту отображать
                    out = String(0.0) + "hz";
                    break;
                }
                return  out; })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setX(4)
            ->setY(3);

        _herz.setButtonId(1)
            ->setSelectedButtonPtr(&selectedButton)
            ->setCalculatedText([this]
                                { 
                                    String out;
                                    if(_isOnSampleChangeMod){
                                    out =  String(_oscil->getMeasuresInSecond()) + "Hz"; 
                                    }
                                    else {
                                    out =  "Smps";
                                    }  
                                    return out; })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setX(4)
            ->setY(_display->getHeight() - 10);

        _pause.setButtonId(2)
            ->setSelectedButtonPtr(&selectedButton)
            ->setCalculatedText([this]
                                { return _oscil->isOnPause() ? ">" : "II"; })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setX(_display->getWidth() - 20)
            ->setY(_display->getHeight() - 10);
    }

    void _initWaveform()
    {
        display_position size = {
            .leftUp{
                .x = 0,
                .y = 0,
            },
            .rightDown{
                .x = _display->getResolution().width,
                .y = _display->getResolution().height}};

        _waveform.setArea(size);
        _waveform.setPointsSource([this]
                                  { return _voltmeter->getMeasures(); });
    }

    void _initWaitText()
    {
        _waitText
            .setText("Подождите")
            ->setPosition(ELEMENT_POSITION_CENTER, ELEMENT_POSITION_CENTER)
            ->setTextSize(EL_TEXT_SIZE_SMALL);
    }

    void onOkPress()
    {
        switch (selectedButton)
        {
        case 0:
            // Сменить отображаемые в углу данные
            _selectedMeasuresMode = range(_selectedMeasuresMode + 1, 0, 2, true);
            break;

        case 1:
            // Войти в режим выбора частоты
            sampleChangeMode(true);
            break;

        case 2:
            // Плей пауза для осцилографа
            _oscil->playPause();
            break;
        }
    }

    void _changeEditSmpsPosition(int32_t changeValue, int8_t maxPos)
    {
        _editSmpsPosition = range(_editSmpsPosition + changeValue, 1, maxPos, true); // Переносим на следующий разряд, ограничивая максимальным и минимальным значением
        _herz.setEditPosition(_editSmpsPosition);                                    // Выводим подчеркивание позиции в интерфейсе
    }

public:
    uint8_t buttonsCount = 3;
    uint8_t selectedButton = 1; // 0 - сменить измеряемые данные
                                // 1 - управление семплированием
                                // 2 - плей/пауза
                                // 3 - открыть подменю

    ElWaveform _waveform;
    ElText _waitText;
    ElText oscilFreqText;
    ElText leftUpInfoText;

    ElTextButton _volt;
    ElTextButton _herz;
    ElTextButton _pause;

    OscilPageView(DisplayVirtual *display, InterfaceEngineVirtual *iEngine, OscilVirtual *oscil, Voltmetr *voltmeter) : PageView(display)
    {
        _oscil = oscil;
        _voltmeter = voltmeter;
        _iEngine = iEngine;

        _initWaveform();
        _initWaitText();
        _initBottomMenu();
        _initInfoTexts();

        addElement(&_waveform)
            ->addElement(&_waitText)
            //->addElement(&_bottomButtons)
            //->addElement(&oscilFreqText)
            ->addElement(&_volt)
            ->addElement(&_herz)
            ->addElement(&_pause) //
            ;

        _lastButtonPressTime = millis();
    }

    bool onControlEvent(control_event_type eventType)
    {
        if (_isOnSampleChangeMod)
        {
            switch (eventType)
            {
            case control_event_type::PRESS_OK:
            {
                int8_t maxPos = getMaxNumPosition<uint32_t>(_oscil->getMeasuresInSecond()); // Максимальное положение

                _changeEditSmpsPosition(-1, maxPos); // Переносим указатель редактирования влево
                break;
            }

            case control_event_type::PRESS_BACK:
                // Выйти из режима редактирования частоты опроса
                sampleChangeMode(false);
                break;

            case control_event_type::PRESS_LEFT:
                // Уменьшить семпл рейт
                changeOscilSamplerate(false);
                break;

            case control_event_type::PRESS_RIGHT:
                // Увеличить семпл рейт
                changeOscilSamplerate(true);
                break;
            }
        }
        else
        {
            switch (eventType)
            {
            case control_event_type::PRESS_OK:
                onOkPress();
                break;

            case control_event_type::PRESS_BACK:
                return false;

            case control_event_type::PRESS_LEFT:
                selectedButton = range(selectedButton - 1, 0, buttonsCount - 1, true);
                break;

            case control_event_type::PRESS_RIGHT:
                selectedButton = range(selectedButton + 1, 0, buttonsCount - 1, true);
                break;
            }
        }

        return true;
    }

    /// @brief Изменить частоту семплирования осцилографа в посимвольном режиме редактирования
    /// @param increase true если увеличить
    /// @return новая частота семплирования
    uint32_t changeOscilSamplerate(bool increase)
    {
        auto currentMsps = _oscil->getMeasuresInSecond(); // Текущая частота считываний

        uint16_t maxPos = getMaxNumPosition<uint32_t>(currentMsps);     // Узнаем максимальную позицию
        int num = pow(10, maxPos - _editSmpsPosition);                  // Вычисляем величину изменения, возводя 10 в степень (10^0=1, 10^1=10, 10^n=1n)
        currentMsps = increase ? currentMsps + num : currentMsps - num; // Вычисляем новое значение частоты

        // Устанавливаем значение частоты опроса в класс осциллографа
        // Он самостоятельно ограничивает минимальную и максимальную частоту
        _oscil->setMeasuresInSecond(currentMsps);

        // Узнаем максимальную позицию после изменения числа
        uint16_t maxPosAfter = getMaxNumPosition<uint32_t>(_oscil->getMeasuresInSecond());

        // Вычисляем разницу
        int16_t diff = maxPosAfter - maxPos;

        // Проверяем, что число позиция ещё входит в необходимые пределы
        // и переносим указатель на другую позицию если изминилось кол-во разрядов
        _changeEditSmpsPosition(diff, maxPosAfter);

        return currentMsps;
    }

    void onDraw()
    {
        // Частота семплирования появляется когда в режиме её изменения и когда
        oscilFreqText.setVisibility(_isOnSampleChangeMod);
    }

    void sampleChangeMode(bool on)
    {
        _isOnSampleChangeMod = on;

        if (on)
        {
            _editSmpsPosition = getMaxNumPosition<uint32_t>(_oscil->getMeasuresInSecond());
            _herz.setEditPosition(_editSmpsPosition);
            Serial.println("smapleChange ON. Start pos: " + String(_editSmpsPosition));
        }
        else
        {
            _herz.setEditPosition(-1);
        }
    }
};