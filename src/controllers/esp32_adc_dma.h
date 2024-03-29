#pragma once

/**
 * @file esp32_adc_dma.h
 * @author jackfrombb (jack@boringbar.ru)
 * @brief Считывание показаний встроенных АЦП через DMA в continue режиме для плат esp32
 * на основе esp-idf 4.4.6 (макимально доступной на время написания).
 *
 * (Проверено:
 * wroom32u - обязательно conv_limit_en = true ,
 * s2mini - conv_limit_en = false, но это ставит ограничение на макс smps в 83khz)
 *
 * - В режиме DMA доступен только ADC1
 * - Если на любой плате не стоит conv_lim_en = true, то семплрейт ограничен: 611Hz ~ 83333Hz,
 * если false то возможный диапазон значительно больше, пока не знаю точных значений (для wroom32 мин: 20000Hz)
 * @version 0.1
 * @date 2024-02-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "adc_virtual.h"
#include "logi.h"
#include "helpers.h"
#include "driver/adc.h"
#include "soc/syscon_periph.h"

// Количество каналов АЦП
// #define ADC_CHANNEL_NUM 1
// Количество преобразований АЦП в одном цикле
// #define ADC_CONVERSIONS 256
// Максимальный размер пула памяти для DMA в байтах
// #define ADC_POOL_SIZE 1024
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

class Esp32AdcDma : public AdcVirtual
{
private:
    bool _enableConvLim = true;
    uint32_t _adc1_channel_mask = BIT(0);
    uint32_t _adc2_channel_mask = 0; // Выключает маску второго ацп
    adc1_channel_t _channel[1] = {};

    uint8_t *_buffer8bit;        // Буфер для непреобразованных значений
    uint16_t _internalBuferSize; // Размер буфера значений с ацп ( = внешний буфер * 2)

public:
    Esp32AdcDma(bool enableConvLim, adc1_channel_t adcChanel)
    {
        _enableConvLim = enableConvLim;
        _adc1_channel_mask = BIT((uint8_t)adcChanel);
        _channel[0] = adcChanel;
    }

    ~Esp32AdcDma() override
    {
        adc_power_release();
    }

    int8_t init(uint16_t bufferSize, uint sampleRate) override
    {
        AdcVirtual::init(bufferSize, sampleRate);

        adc_power_acquire();

        _internalBuferSize = bufferSize << 1;
        _buffer8bit = (uint8_t *)calloc(_internalBuferSize, sizeof(uint8_t));

        adc_digi_init_config_t adc_config = adc_digi_init_config_t{
            .max_store_buf_size = (uint32_t)_internalBuferSize * 2,
            .conv_num_each_intr = (uint32_t)_bufferSize,
            .adc1_chan_mask = _adc1_channel_mask,
            .adc2_chan_mask = _adc2_channel_mask,
        };

        if (logi::err("Esp32_adc_dma - initAdcDigi", adc_digi_initialize(&adc_config)))
        {
            logi::p("Esp32_adc_dma", "Init adc digi OK");
        }

        adc_digi_configuration_t con = adc_digi_configuration_t{
            .conv_limit_en = _enableConvLim, // Для обычных esp32 это всегда true
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
            uint8_t unit = GET_UNIT(_channel[i]);
            uint8_t ch = _channel[i] & 0x7;
            adc_pattern[i].atten = ADC_ATTEN_DB_11;
            adc_pattern[i].channel = ch;
            adc_pattern[i].unit = unit;
            adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

            // ESP_LOGI("Esp32_adc_dma", "adc_pattern[%d].atten is :%x", i, adc_pattern[i].atten);
            // ESP_LOGI("Esp32_adc_dma", "adc_pattern[%d].channel is :%x", i, adc_pattern[i].channel);
            // ESP_LOGI("Esp32_adc_dma", "adc_pattern[%d].unit is :%x", i, adc_pattern[i].unit);
        }

        con.adc_pattern = adc_pattern;

        auto ret = adc_digi_controller_configure(&con);

        // adc_digi_monitor_t monitor = adc_digi_monitor_t{
        //     .adc_unit = ADC_UNIT_1,
        //     .channel = ADC_CHANNEL_6,
        //     .mode = adc_digi_monitor_mode_t::ADC_DIGI_MONITOR_MAX,
        //     .threshold = 500,
        // };

        // adc_digi_monitor_set_config(adc_digi_monitor_idx_t::ADC_DIGI_MONITOR_IDX0, &monitor);
        // adc_digi_monitor_enable(adc_digi_monitor_idx_t::ADC_DIGI_MONITOR_IDX0, true);

        if (logi::err("Esp32_adc_dma - controller configure", ret))
        {
            Serial.println("Err val: " + String(ret));
            return adc_digi_start();
        }
        
        return ret;
    }

    int8_t deinit() override
    {
        free(_buffer8bit);
        logi::err("Esp32Board - adc digi stop", adc_digi_stop());
        return adc_digi_deinitialize();
    }

    int8_t changeSampleRate(uint sampleRate) override
    {
        deinit();
        AdcVirtual::changeSampleRate(sampleRate);
        return init(_bufferSize, sampleRate);
    }

    bool isStarted = false;
    int8_t readData(uint16_t *buffer, size_t *readedLength) override
    {
        // Метод возвращает ESP_ERR_INVALID_STATE, но работает. Не знаю почему так происходит
        auto retErr = adc_digi_read_bytes(_buffer8bit, _internalBuferSize, readedLength, 10);
        // logi::p("Adc_dma", "Err: " + String(retErr));

        for (int i = 0; i < *readedLength; i += ADC_RESULT_SIZE)
        {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)&_buffer8bit[i];
            if (_check_valid_data(p))
            {
                buffer[i >> 1] = p->type1.data;
            }
        }
        *readedLength = *readedLength >> 1; // Делим колво байт на 2 что бы сходились последующие расчеты
        // adc_digi_stop();
        return 0;
    }

    /// @brief Получить максимальную скорость семплирования с ADC для платы
    /// @return Скорость семплирования ADC
    uint getMaxAdcSampleRate() override
    {
        if (_enableConvLim)
        {
            return 1500000; // 1.5mhz
        }
        return 83333; // Стандартно для adc dma без ограничения по счтыванию
    }

    /// @brief Получить минимальную скорость семплирования с ADC для платы
    /// @return Скорость семплирования ADC
    virtual uint16_t getMinAdcSampleRate() override
    {
        if (_enableConvLim)
            return 20000; // Обязательно для wroom32

        return 3000; // Стандартно для adc dma с conv_limit_en = false. Возможно сделаю переход на single read, тогда нижняя планка может стать 1
        // Почему то ниже чем 3000 ошибка
    }

private:
    static bool _check_valid_data(const adc_digi_output_data_t *data)
    {
        if (data->type1.channel >= SOC_ADC_CHANNEL_NUM(ADC_UNIT_1))
        {
            return false;
        }
        return true;
    }
};