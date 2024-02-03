#pragma once

#include "controllers/esp32_adc_dma.h"
#include "esp32_virtual.h"
//#include "logi.h"

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 1
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 1024
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

class Esp32S2Mini : public Esp32Virtual
{
protected:
    const init_adc_info _adcInfo = {
        .unit = (adc_unit_t)GET_UNIT(ADC1_CHANNEL_4),
        .chanelAdc1 = ADC1_CHANNEL_4,
        .atten = ADC_ATTEN_DB_11,
        .width = ADC_WIDTH_BIT_13,
    };

    // Маска каналов АЦП
    // uint32_t adc_channel_mask = BIT(4);

    uint32_t adc1_channel_mask = BIT(4);
    uint32_t adc2_channel_mask = 0;
    adc1_channel_t channel[1] = {
        ADC1_CHANNEL_4,
    };

    // Для отображения семплов в секунду
    uint16_t _outBufferSize = 512;

    // Буферы для хранения результатов АЦП
    // uint8_t adc_buffer[ADC_BUFFER_SIZE] = {0};      // Буфер для байтов замеров
    // uint16_t adc_buffer_out[ADC_BUFFER_SIZE] = {0}; // Буфер для полуения показаний от 0-4096

    // Параметры АЦП
    // adc_digi_pattern_config_t adc_patern;
    adc_digi_configuration_t con;
    adc_digi_init_config_t adc_config;

private:
    uint8_t *buffer8bit;

    static bool _check_valid_data(const adc_digi_output_data_t *data)
    {
        if (data->type1.channel >= SOC_ADC_CHANNEL_NUM(ADC_UNIT_1))
        {
            return false;
        }
        return true;
    }

public:
    Esp32S2Mini(DisplayVirtual *display, ControlVirtual *control) : Esp32Virtual(display, control)
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
            _adc = new Esp32AdcDma(false, _adcInfo.chanelAdc1);
        }

        return _adc;
    }

    virtual uint16_t getPwmPin()
    {
        return GPIO_NUM_17;
    }
};