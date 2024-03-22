#pragma once

// esp32 библиотеки для работы ADC
// #include "module_virtual.h"
// #include "displays/display_virtual.h"
// #include "controls/control_virtual.h"
// #include "interface/engines/interface_engine.h"

#define PROBE_COUNT 256 // Кол-во проб для усредненой калибровки ацп
#define REF_VOLT 2.504  // Референсное напряжение

/// @brief Основная плата устройства
class MainBoard : public ModuleVirtual
{
protected:
    DisplayVirtual *_display;
    ControlVirtual *_control;
    InterfaceEngineVirtual *_interfaceEngine;

    uint _sampleRate;
    uint16_t _bufferSize;
    uint16_t _rawReferenceVal_2_5 = 0;
    double _calibrationVal = 0.0000;

    AdcVirtual *_adc = nullptr;
    PwmControllerVirtual *_pwm = nullptr;

private:
    uint16_t counter = 0; // счётчик
    float prevResult = 0; // хранит предыдущее готовое значение
    float sum = 0;        // сумма
    /// Обычное среднее (чуть подогнанный метод с сайта Алекса Гайвера)
    template <typename T>
    T midArifm2(T newVal, T measureSize)
    {
        sum += newVal; // суммируем новое значение
        counter++;     // счётчик++

        if (counter == measureSize)
        {                                   // достигли кол-ва измерений
            prevResult = sum / measureSize; // считаем среднее
            sum = 0;                        // обнуляем сумму
            counter = 0;                    // сброс счётчика

            // logi::p("Helpers", "RESSET; Prev: " + String(prevResult));
        }
        // logi::p("Helpers", "SUM: " + String(sum) + " Prev result: " + String(prevResult) + " Counter: " + String(counter));
        return (T)prevResult;
    }

public:
    MainBoard(DisplayVirtual *display, ControlVirtual *control)
    {
        _display = display;
        _control = control;
        _interfaceEngine = display->getInterfaceEngine();
    }

    virtual ~MainBoard()
    {
        /*
        Этот метод ни когда не будет вызываться, но все же напишу
        */
        delete _adc;
        delete _pwm;
    }

    virtual void switchRefVoltage(bool on)
    {
    }

    float maxAdcVolt = 33;
    virtual float getMaxAdcVolt(){
        return maxAdcVolt;
    }

    virtual void setReferenceVal(uint16_t refVal)
    {
        _rawReferenceVal_2_5 = refVal;
        auto curVal = (((double)refVal / (static_cast<double>((1 << 12) - 1))) * 3.3) * 1000.0;

        _calibrationVal = (6.330 * 1000.0) / curVal;

        maxAdcVolt = (4095.0 / refVal) * 6.33;

        Serial.println("CalibVal: " + String(_calibrationVal) +
                       " Raw val " + String(_rawReferenceVal_2_5) + " Cur val: " + String(curVal));
    }

    virtual double getCalibrationVal()
    {
        return _calibrationVal;
    }

    virtual uint16_t getRawReferenceVal()
    {
        return _rawReferenceVal_2_5;
    }

    virtual void calibrate()
    {
        //setReferenceVal(985000.0/99950.0);
        if (getCalibrationPin() > 0)
        {
            // switchRefVoltage(true);
            // initAdc_SingleRead();
            // uint16_t val = 0;
            // for (uint16_t i = 0; i <= PROBE_COUNT; i++)
            // {
            //     delay(1);
            //     auto raw = readAdc_Single();
            //     val = midArifm2<uint16_t>(raw, PROBE_COUNT);
            // }
            // setReferenceVal(val);
            // deinitAdc_SingleRead();
            // switchRefVoltage(false);
        }
    }

    virtual void init()
    {
        _pwm->settingsInit();
    }

    DisplayVirtual *getDisplay()
    {
        return _display;
    }

    ControlVirtual *getControl()
    {
        return _control;
    }

    InterfaceEngineVirtual *getInterfaceEngine()
    {
        return _interfaceEngine;
    }

    virtual AdcVirtual *getAdcContinue()
    {
        return _adc;
    }

    virtual PwmControllerVirtual *getPwmController()
    {
        return _pwm;
    }

    virtual void removeAdcContinue()
    {
        delete _adc;
        _adc = nullptr;
    }

    virtual uint16_t readAdc_Single()
    {
        return 0;
    }

    virtual esp_err_t initAdc_SingleRead()
    {
        return ESP_OK;
    }

    virtual esp_err_t deinitAdc_SingleRead()
    {
        return ESP_OK;
    }

    virtual esp_err_t changeSampleRate(uint sampleRate)
    {
        _sampleRate = sampleRate;
        return ESP_OK;
    }

    virtual uint getSampleRate()
    {
        return _sampleRate;
    }

    virtual uint32_t rawToVoltage(uint16_t reading)
    {
        // Тут по сути заглушка, этот метод переопределяется в классах плат
        return (uint32_t)(((float)reading * (3.3 / 4095.0)) * 1000);
    }

    virtual int8_t getCalibrationPin()
    {
        return -1;
    }

    virtual uint16_t getPwmPin() = 0;
};