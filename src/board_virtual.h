#pragma once

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2s.h"
#include "module_virtual.h"
#include "displays/display_virtual.h"
#include "controls/control_virtual.h"

/// @brief Основная плата устройства
class MainBoard : public ModuleVirtual
{
protected:
    DisplayVirtual *_display;
    ControlVirtual *_control;

    uint _sampleRate;
    uint16_t _bufferSize;

private:
public:
    MainBoard(DisplayVirtual *display, ControlVirtual *control)
    {
        _display = display;
        _control = control;
    }

    virtual ~MainBoard() = default;

    virtual void init()
    {
    }

    DisplayVirtual *getDisplay()
    {
        return _display;
    }

    ControlVirtual *getControl()
    {
        return _control;
    }

    virtual uint16_t readAdc_Single()
    {
        return 0;
    }

    virtual esp_err_t readAdc_Continue(uint16_t *buffer, size_t *readLenght)
    {
        return 0;
    }

    virtual esp_err_t initAdc_Continue(uint16_t bufferSize, uint sampleRate)
    {
        _sampleRate = sampleRate;
        _bufferSize = bufferSize;
        return ESP_OK;
    }

    virtual esp_err_t deinitAdc_Continue()
    {
        return ESP_OK;
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
        //Тут по сути заглушка, этот метод переопределяется в классах плат
        // if (getAdcChars() != nullptr)
        //   return esp_adc_cal_raw_to_voltage(reading, getAdcChars()); // reading * 3.3 / 4096.0; // esp_adc_cal_raw_to_voltage(reading, getAdcChars());
        // else
        return (uint32_t)((float)reading * (3.2 / 4095)) * 1000;
    }

    virtual uint16_t getPwmPin() = 0;
};