#pragma once
/**
 * @file esp32_adc_i2s.h
 * @author jackfrombb (jack@boringbar.ru)
 * @brief Реализация считывания с АЦП через I2S. Доступно только для стандартных esp32 (не s2, с3  и пр)
 * - АЦП и ЦАП поддерживают только I2S номер 0, потому не могут использоваться одновременно
 * - При использовании ADC2 не работает wifi, потому я не предполагаю его использование
 * @version 0.1
 * @date 2024-02-01
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "adc_virtual.h"
#include "driver/i2s.h"
#include "soc/syscon_periph.h"
#include "logi.h"

#define OSCIL_I2S_NUM I2S_NUM_0

class Esp32AdcI2s : public AdcVirtual
{
private:
    adc1_channel_t _adcChanel;

public:
    Esp32AdcI2s(adc1_channel_t adcChanel)
    {
        _adcChanel = adcChanel;
    }
    ~Esp32AdcI2s()
    {
    }

    int8_t init(uint16_t bufferSize, uint sampleRate) override
    {
        AdcVirtual::init(bufferSize, sampleRate);
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
            .sample_rate = _sampleRate,                   // The format of the signal using ADC_BUILT_IN
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
            .channel_format = I2S_CHANNEL_FMT_ALL_LEFT,
            .communication_format = i2s_comm_format_t::I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,        // number of DMA buffers
            .dma_buf_len = bufferSize, // number of samples (in bytes)
            .use_apll = false,         // no Audio PLL
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0};

        i2s_driver_install(OSCIL_I2S_NUM, &i2s_config, 0, NULL);
        i2s_set_adc_mode(ADC_UNIT_1, _adcChanel);

        logi::p("Esp32Board", "I2s init ok");

        SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
        SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table
                                              // REG_SET_BIT(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_DATA_TO_I2S);

        // delay(300); // required for stability of ADC (вроде и без задержек работает норм, но оставлю как напоминание)

        auto i2s_adc_err = i2s_adc_enable(OSCIL_I2S_NUM);
        logi::err("Oscil i2s", i2s_adc_err);
        return i2s_adc_err;
    }

    int8_t deinit() override
    {
        if (logi::err("Esp32Board", i2s_adc_disable(OSCIL_I2S_NUM)))
        {
            Serial.println("Disable i2s adc ok");
        }
        return i2s_driver_uninstall(OSCIL_I2S_NUM);
    }

    int8_t changeSampleRate(uint sampleRate) override
    {
        AdcVirtual::changeSampleRate(sampleRate);
        _sampleRate = sampleRate;
        return i2s_set_sample_rates(OSCIL_I2S_NUM, sampleRate);
    }

    int8_t readData(uint16_t *buffer, size_t *readedLength) override
    {
        auto err = i2s_read(OSCIL_I2S_NUM, buffer, _bufferSize, readedLength, portMAX_DELAY);
        // delayMicroseconds(1000);
        *readedLength = *readedLength >> 1; // Не знаю почему, но он заполняет весь массив,
                                            // если делить кол-во считанных байт на 2
        return err;
    }
};