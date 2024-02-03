#pragma once

//#include "page_virtual.h"
//#include "interface/pages/views/wide_screen/voltmeter_page_view.h"

class VoltmeterPage : public InterfacePageVirtual
{
private:
    // Представление
    VoltemeterPageView *_pageView;

    // Логика измерителей
    OscilVirtual *_oscil = nullptr;
    Voltmetr *_voltmeter = nullptr;

    // Вывод данных
    String *_voltTextPtr = nullptr;
    String *_secondVoltTextPtr = nullptr;
    String *_samplerateTextPtr = nullptr;
    adc_measures_t *_measuresWaveformPtr = nullptr;

    //uint bias = 0;

public:
    VoltmeterPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        // Устанавливаем view
        _pageView = new VoltemeterPageView(mainBoard->getDisplay(), mainBoard->getInterfaceEngine());

        // Восстанавливаем последнее значение частоты АЦП
        uint lastSampleRate = AppData::getUint("lastSampleData", 80000);
        Serial.println("Get uint: " + String(lastSampleRate));

        // Создаем логику измерения
        _oscil = new OscilAdcDma(mainBoard, lastSampleRate);
        _voltmeter = new Voltmetr(mainBoard);
        _voltmeter->setOscil(_oscil);
        _oscil->init();

        // Инициализируем поля данных
        _voltTextPtr = _pageView->getVoltPattern();
        _measuresWaveformPtr = _pageView->getWaveformMeasuresPtr();
        _secondVoltTextPtr = _pageView->getSecondVoltTextPtr();
        _samplerateTextPtr = _pageView->getSampleRateTextPtr();
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
        //measures.bias = bias;

        *_measuresWaveformPtr = measures;
        *_voltTextPtr = String(((float)measures.max / 1000.0)); // Делим на 1000 что бы получить вольты
        *_secondVoltTextPtr = "mid" + String(((float)measures.middle / 1000.0));
        *_samplerateTextPtr = "min" + String(((float)measures.min / 1000.0));
    }
};