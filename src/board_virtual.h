#pragma once

// esp32 библиотеки для работы ADC
// #include "module_virtual.h"
// #include "displays/display_virtual.h"
// #include "controls/control_virtual.h"
// #include "interface/engines/interface_engine.h"

/// @brief Основная плата устройства
class MainBoard : public ModuleVirtual
{
protected:
    DisplayVirtual *_display;
    ControlVirtual *_control;
    InterfaceEngineVirtual *_interfaceEngine;

    uint _sampleRate;
    uint16_t _bufferSize;

    AdcVirtual *_adc = nullptr;
    PwmControllerVirtual *_pwm = nullptr;

private:
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
        return (uint32_t)(((float)reading * (3.2 / 4095.0)) * 1000);
    }

    virtual uint16_t getPwmPin() = 0;
};