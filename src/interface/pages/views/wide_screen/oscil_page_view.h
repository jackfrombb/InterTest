#pragma once
#include "interface/pages/views/page_view.h"
using namespace std;

class OscilPageView : public PageView
{
private:
    ElCenteredGroup _bottomButtons;
    OscilVirtual *_oscil;

    display_position *_bottomMenuPosition; // Указатель на позицию меню, для анимации перемещения
    bool _showBottomMenuTrigger = true;    // Триггер для отображения меню
    int _bottomMenuPositionOnShow;         // Изначальное, нормальное положение меню

    ulong _lastButtonPressTime;
    bool _isOnSampleChangeMod = false;

    uint8_t _selectedMeasuresMode = 0;

    void _initInfoTexts()
    {
        oscilFreqText.setCalculatedText([this]()
                                        { return String(_oscil->getMeasuresInSecond()); })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setX(0)
            ->setY(_bottomMenuPositionOnShow)
            ->setWidth(_display->getWidth())
            ->setVisibility(false);
    }

    void _initBottomMenu()
    {

        _volt.setButtonId(0)
            ->setSelectedButtonPtr(&selectedButton)
            ->setText("Vm")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(20)
            ->setHeight(10);

        _herz.setButtonId(1)
            ->setSelectedButtonPtr(&selectedButton)
            ->setText("Hz")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(30)
            ->setHeight(10);

        _pause.setButtonId(2)
            ->setSelectedButtonPtr(&selectedButton)
            ->setText("P")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(30)
            ->setHeight(10);

        int16_t paddingW = 2;
        int16_t paddingBottom = 2;

        _bottomButtons
            .setX(paddingW)
            ->setY(_display->getHeight() - (_volt.getHeight() + 4 + paddingBottom))
            ->setWidth(_display->getWidth() - paddingW)
            ->setHeight(_volt.getHeight() + 4);

        _bottomButtons.addElement(&_volt)->addElement(&_herz)->addElement(&_pause);

        _bottomMenuPosition = _bottomButtons.getAreaPtr();
        _bottomMenuPositionOnShow = _bottomButtons.getArea().getY();
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

        //_waveform.setPoints(_oscil->getBuffer(), _oscil->getBufferLength());
        _waveform.setPointsSource([this]
                                  { return _oscil->getBuffer(); },
                                  _oscil->getBufferLength());

        _waveform.setOscil(_oscil);
    }

    void _initWaitText()
    {
        _waitText.setText("Подождите")->setPosition(ELEMENT_POSITION_CENTER, ELEMENT_POSITION_CENTER)->setTextSize(EL_TEXT_SIZE_SMALL);
    }

    void onOkPress()
    {
        switch (selectedButton)
        {
        case 0:
            // Сменить отображаемые в углу данные
            _selectedMeasuresMode = range(_selectedMeasuresMode, 0, 2, true);
            break;

        case 1:
            // Войти в режим выбора частоты
            sampleChangeMode(true);
            hideBottomMenu();
            break;

        case 2:
            // Плей пауза для осцилографа
            if (_oscil->playPause())
            {
                hideBottomMenu();
            }
            else
            {
                showBottomMenu();
            }
            break;
        }
    }

public:
    uint8_t buttonsCount = 3;
    uint8_t selectedButton = 1; // 0 - сменить измеряемые данные
                                // 1 - управление семплированием
                                // 2 - плей/пауза

    ElWaveform <uint16_t> _waveform;
    ElText _waitText;
    ElText oscilFreqText;
    ElText leftUpInfoText;

    ElTextButton _volt;
    ElTextButton _herz;
    ElTextButton _pause;

    OscilPageView(DisplayVirtual *display, OscilVirtual *oscil) : PageView(display)
    {
        _oscil = oscil;

        _initWaveform();
        _initWaitText();
        _initBottomMenu();
        _initInfoTexts();

        addElement(&_waveform)->addElement(&_waitText)->addElement(&_bottomButtons)->addElement(&oscilFreqText);

        _lastButtonPressTime = millis();
    }

    void onControlEvent(control_event_type eventType)
    {
        if (_isOnSampleChangeMod)
        {
            switch (eventType)
            {
            case control_event_type::PRESS_OK:
                sampleChangeMode(false);
                showBottomMenu();
                break;

            case control_event_type::PRESS_LEFT:
                // Уменьшить семпл рейт
                changeOscilMeasures(false, 1);
                break;

            case control_event_type::PRESS_RIGHT:
                // Увеличить семпл рейт
                changeOscilMeasures(true, 1);
                break;
            }
        }
        else
        {
            showBottomMenu();

            switch (eventType)
            {
            case control_event_type::PRESS_OK:
                onOkPress();
                break;

            case control_event_type::PRESS_LEFT:
                selectedButton = range(selectedButton - 1, 0, buttonsCount - 1, true);
                break;

            case control_event_type::PRESS_RIGHT:
                selectedButton = range(selectedButton + 1, 0, buttonsCount - 1, true);
                break;
            }
        }
    }

    uint32_t changeOscilMeasures(bool increase, int16_t multipler)
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

        if (_showBottomMenuTrigger && _bottomMenuPosition->getY() > _bottomMenuPositionOnShow)
        {
            if (!_bottomButtons.isVisible())
            {
                _bottomButtons.setVisibility(true);
            }

            _bottomMenuPosition->leftUp.y -= 2;
            _bottomMenuPosition->rightDown.y -= 2;
        }
        else if (!_showBottomMenuTrigger && _bottomMenuPosition->getY() < _display->getResolution().height)
        {
            _bottomMenuPosition->leftUp.y += 2;
            _bottomMenuPosition->rightDown.y += 2;

            if (_display->getResolution().height <= _bottomMenuPosition->leftUp.y)
            {
                _bottomButtons.setVisibility(false);
            }
        }

        if (_showBottomMenuTrigger && millis() - _lastButtonPressTime > 3000)
        {
            hideBottomMenu();
        }
    }

    void sampleChangeMode(bool on)
    {
        _isOnSampleChangeMod = on;
    }

    void showBottomMenu()
    {
        _lastButtonPressTime = millis();
        _showBottomMenuTrigger = true;
    }

    void hideBottomMenu()
    {
        _showBottomMenuTrigger = false;
    }
};