#pragma once
#include "interface/pages/views/page_view.h"
#include "voltmeter.h"

using namespace std;

class OscilPageView : public PageView
{
private:
    ElCenteredGroup _bottomButtons;
    OscilVirtual *_oscil;
    Voltmetr *_voltmeter;
    InterfaceEngineVirtual *_iEngine;

    bool _isOnSampleChangeMod = false;
    uint16_t _smapleChangeMultipler = 1;

    uint8_t _selectedMeasuresMode = 0; // 0 - Пик ту пик, 1 - среднее, 2 - герцы

    int _defaultBottomMenuPosition; // Изначальное, нормальное положение меню

    ulong _lastButtonPressTime;

    void _initInfoTexts()
    {
        oscilFreqText.setCalculatedText([this]()
                                        { return String(_oscil->getMeasuresInSecond()); })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
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
                _smapleChangeMultipler = range(_smapleChangeMultipler * 10, 1, _oscil->getMeasuresInSecond(), true);
                sampleChangeMode(true);
                break;

            case control_event_type::PRESS_BACK:
                sampleChangeMode(false);
                break;

            case control_event_type::PRESS_LEFT:
                // Уменьшить семпл рейт
                changeOscilSamplerate(false, _smapleChangeMultipler);
                break;

            case control_event_type::PRESS_RIGHT:
                // Увеличить семпл рейт
                changeOscilSamplerate(true, _smapleChangeMultipler);
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

    uint32_t changeOscilSamplerate(bool increase, int16_t multipler)
    {
        auto t = _oscil->getMeasuresInSecond();
        t = increase ? t + (1 * multipler) : t - (1 * multipler);

        if (t > 500)
            _oscil->setMeasuresInSecond(t);

        return t;
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
            auto multiplerPos = getMaxNumPosition<uint32_t>(_smapleChangeMultipler);
            auto maxPos = getMaxNumPosition<uint32_t>(_oscil->getMeasuresInSecond());
            _herz.setEditPosition(maxPos - (multiplerPos - 1));
        }
        else
        {
            _herz.setEditPosition(-1);
        }
    }
};