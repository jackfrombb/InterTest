#pragma once

#include <Arduino.h>
#include "esp32_virtual.h"
#include "logi.h"
#include "helpers.h"
#include "driver/rtc_io.h"

#define CALIBRATION_PIN GPIO_NUM_19

#define ADC_I2S_MODE // i2s для wroom32 показывает меньше шумов и вообще лучше работает по ощущениям
// #define ADC_DMA_MODE

// Временный костыль переключающий доступные режимы ацп
// В планах или оставить один, или сделать переключение в настройках, для тех плат где оно доступно
#ifdef ADC_DMA_MODE
#include "controllers/esp32_adc_dma.h"
#elif defined ADC_I2S_MODE
#include "controllers/esp32_adc_i2s.h"
#endif

class Esp32Wroom32 : public Esp32Virtual
{
protected:
    const init_adc_info _adcInfo = {
        .unit = ADC_UNIT_1,
        .chanelAdc1 = ADC1_CHANNEL_7,
        .atten = ADC_ATTEN_11db,
        .width = ADC_WIDTH_12Bit,
    };

public:
    Esp32Wroom32(DisplayVirtual *display, ControlVirtual *control) : Esp32Virtual(display, control)
    {
    }

    void init() override
    {
        gpio_reset_pin((gpio_num_t)getCalibrationPin());
        delay(100);
        gpio_set_direction((gpio_num_t)getCalibrationPin(), gpio_mode_t::GPIO_MODE_OUTPUT);
        Esp32Virtual::init();
    }

    void switchRefVoltage(bool on) override
    {
        if (on)
        {
            gpio_set_level((gpio_num_t)getCalibrationPin(), 1);
        }
        else
        {
            gpio_set_level((gpio_num_t)getCalibrationPin(), 0);
        }
    }

    init_adc_info getAdcInfo() override
    {
        return _adcInfo;
    }

    AdcVirtual *getAdcContinue() override
    {
        if (_adc == nullptr)
        {
#ifdef ADC_I2S_MODE
            _adc = new Esp32AdcI2s(_adcInfo.chanelAdc1);
#elif defined(ADC_DMA_MODE)
            _adc = new Esp32AdcDma(true, _adcInfo.chanelAdc1);
#endif
        }

        return _adc;
    }

    int8_t getCalibrationPin() override
    {
        return CALIBRATION_PIN;
    }

    uint16_t getPwmPin() override
    {
        return GPIO_NUM_2;
    }
};