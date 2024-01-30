#pragma once

#include "esp32_virtual.h"
#include "logi.h"

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
    uint8_t adc_buffer[ADC_BUFFER_SIZE] = {0};      // Буфер для байтов замеров
    uint16_t adc_buffer_out[ADC_BUFFER_SIZE] = {0}; // Буфер для полуения показаний от 0-4096

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

    // Как пример из инета, не для использования
    static void _continuous_adc_init(uint16_t adc1_chan_mask, adc_channel_t *channel, uint8_t channel_num)
    {
        adc_digi_init_config_t adc_dma_config = {
            .max_store_buf_size = 6 * 512,
            .conv_num_each_intr = 512,
            .adc1_chan_mask = adc1_chan_mask,
            .adc2_chan_mask = 0,
        };

        ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

        adc_digi_configuration_t dig_cfg = {
            .conv_limit_en = false,
            .conv_limit_num = 250,
            .sample_freq_hz = 83333,
            //.sample_freq_hz =  1000,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };

        adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
        dig_cfg.pattern_num = channel_num;

        for (int i = 0; i < channel_num; i++)
        {
            uint8_t unit = GET_UNIT(channel[i]);
            uint8_t ch = channel[i] & 0x7;
            adc_pattern[i].atten = ADC_ATTEN_DB_11;
            adc_pattern[i].channel = ch;
            adc_pattern[i].unit = unit;
            adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH; // 11 data bits limit
        }
        dig_cfg.adc_pattern = adc_pattern;
        ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
    }

public:
    Esp32S2Mini(DisplayVirtual *display, ControlVirtual *control) : Esp32Virtual(display, control)
    {
    }

    init_adc_info getAdcInfo() override
    {
        return _adcInfo;
    }

    esp_err_t initAdc_Continue(uint16_t bufferSize, uint sampleRate)
    {
        // initAdc_SingleRead();

        _sampleRate = sampleRate;
        _outBufferSize = bufferSize;
        _bufferSize = bufferSize * 2;

        buffer8bit = (uint8_t *)calloc(_bufferSize, sizeof(uint8_t));

        adc_config = adc_digi_init_config_t{
            .max_store_buf_size = (uint32_t)(_bufferSize),
            .conv_num_each_intr = (uint32_t)_bufferSize,
            .adc1_chan_mask = adc1_channel_mask,
            .adc2_chan_mask = adc2_channel_mask,
        };

        Serial.println("Init OK");

        logi::err("Esp32Board - initAdcDigi", adc_digi_initialize(&adc_config));

        con = adc_digi_configuration_t{
            .conv_limit_en = false,
            .conv_limit_num = 250,
            .sample_freq_hz = (uint32_t)_sampleRate,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };

        uint32_t pattern_num = 1;

        adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
        con.pattern_num = pattern_num;

        for (int i = 0; i < pattern_num; i++)
        {
            uint8_t unit = GET_UNIT(channel[i]);
            uint8_t ch = channel[i] & 0x7;
            adc_pattern[i].atten = ADC_ATTEN_DB_11;
            adc_pattern[i].channel = ch;
            adc_pattern[i].unit = unit;
            adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

            ESP_LOGI("esp32s2", "adc_pattern[%d].atten is :%x", i, adc_pattern[i].atten);
            ESP_LOGI("esp32s2", "adc_pattern[%d].channel is :%x", i, adc_pattern[i].channel);
            ESP_LOGI("esp32s2", "adc_pattern[%d].unit is :%x", i, adc_pattern[i].unit);
        }

        con.adc_pattern = adc_pattern;

        auto ret = adc_digi_controller_configure(&con);
        if (logi::err("Esp32Board - controller configure", ret))
        {
            Serial.println("Configure OK");
            return adc_digi_start();
        }

        return ret;
    }

    esp_err_t readAdc_Continue(uint16_t *buffer, uint16_t *readLenght) override
    {
        auto retErr = adc_digi_read_bytes(buffer8bit, _bufferSize, (size_t *)readLenght, ADC_MAX_DELAY);

        for (int i = 0; i < *readLenght; i += ADC_RESULT_SIZE)
        {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)&buffer8bit[i];

            if (_check_valid_data(p))
            {
                buffer[i >> 1] = p->type1.data;
            }
        }
        // Старый способ, подходил для wroom32u
        // for (int i = 0; i < *readLenght; i += ADC_RESULT_SIZE)
        // {
        //     //  Совмещаем байты для получения показаний
        //     buffer[(int)(i >> 1)] = buffer8bit[i] << 8 | buffer8bit[i + 1];
        // }

        return ESP_OK;
    }

    esp_err_t deinitAdc_Continue() override
    {
        logi::err("Esp32Board - adc digi stop", adc_digi_stop());
        return logi::err("Esp32Board - adc digi deinit", adc_digi_deinitialize());
    }

    esp_err_t changeSampleRate(uint sampleRate)
    {
        deinitAdc_Continue();
        Esp32Virtual::changeSampleRate(sampleRate);
        return initAdc_Continue(_outBufferSize, sampleRate);
    }

    virtual uint16_t getPwmPin() {
        return GPIO_NUM_17;
    }
};