#pragma once

#include "esp32_virtual.h"
#include "logi.h"
#include "helpers.h"

#define ADC_I2S_MODE
//#define ADC_DMA_MODE


//Временный костыль переключающий доступные режимы ацп
//В планах или оставить один, или сделать переключение в настройках, для тех плат где оно доступно
#ifdef ADC_DMA_MODE
#include "controllers/esp32_adc_dma.h"
#elif defined ADC_I2S_MODE
#include "controllers/esp32_adc_i2s.h"
#endif

class Esp32Wroom32 : public Esp32Virtual
{
private:
    const init_adc_info _adcInfo = {
        .unit = ADC_UNIT_1,
        .chanelAdc1 = ADC1_CHANNEL_0,
        .atten = ADC_ATTEN_11db,
        .width = ADC_WIDTH_12Bit,
    };

public:
    Esp32Wroom32(DisplayVirtual *display, ControlVirtual *control) : Esp32Virtual(display, control)
    {
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

    uint16_t getPwmPin() override
    {
        return DAC1;
    }
};