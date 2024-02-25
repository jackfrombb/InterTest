#pragma once

// #include "page_virtual.h"
// #include "interface/pages/views/wide_screen/voltmeter_page_view.h"

class VoltmeterPage : public InterfacePageVirtual
{
private:
    // Представление
    VoltemeterPageView *_pageView;

    // Логика измерителей
    OscilVirtual *_oscil = nullptr;
    Voltmetr *_voltmeter = nullptr;

    // Вывод данных
    String *_bigCenterTextPtr = nullptr;            // Главный центральный
    String *_smallLeftTextPtr = nullptr;            // Слева маленький
    String *_smallRightTextPtr = nullptr;           // Справа маленький
    adc_measures_t *_measuresWaveformPtr = nullptr; // Измерения с adc


public:
    VoltmeterPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        // Устанавливаем view
        _pageView = new VoltemeterPageView(mainBoard->getDisplay(), mainBoard->getInterfaceEngine());

        // Восстанавливаем последнее значение частоты АЦП
        // uint lastSampleRate = AppData::getUint("lastSampleData", 80000);
        // Serial.println("Get uint: " + String(lastSampleRate));

        // Создаем логику измерения
        _oscil = new OscilLogic(mainBoard);
        _voltmeter = new Voltmetr(mainBoard);
        _voltmeter->setOscil(_oscil);
        _oscil->init();

        // Инициализируем поля данных
        _bigCenterTextPtr = _pageView->getBigCenterTextPattern();
        _measuresWaveformPtr = _pageView->getWaveformMeasuresPtr();
        _smallLeftTextPtr = _pageView->getSmallLeftTextPtr();
        _smallRightTextPtr = _pageView->getSampleRateTextPtr();
    }

    ~VoltmeterPage()
    {
        // Очищаем что создали через new
        delete _pageView;
        delete _oscil;
        delete _voltmeter;
        _pageView = nullptr;
        _oscil = nullptr;
        _voltmeter = nullptr;
    }

    PageView *getPageView() override
    {
        return _pageView;
    }

    bool onControlEvent(control_event_type eventType) override
    {
        switch (eventType)
        {
            // Смотрю на смещения буфера
            // case control_event_type::PRESS_LEFT:
            //     bias -= 1;
            //     break;
            // case control_event_type::PRESS_RIGHT:
            //     bias = range(bias + 1, 0, _oscil->getBufferLength() - _display->getWidth());
            //     break;

        case control_event_type::PRESS_BACK:
            return false;

        default:
            break;
        }

        return true;
    }

    void onDraw() override
    {
        auto measures = _voltmeter->getMeasures();
        // measures.bias = bias;

        *_measuresWaveformPtr = measures;
        *_bigCenterTextPtr = String(((float)measures.max / 1000.0)); // Делим на 1000 что бы получить вольты
        *_smallLeftTextPtr = String(((float)measures.middle / 1000.0));
        *_smallRightTextPtr = String(((float)measures.min / 1000.0));

    }
};