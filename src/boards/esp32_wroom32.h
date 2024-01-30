#pragma once

#include "esp32_virtual.h"
#include "logi.h"
#include "helpers.h"
#include "driver/i2s.h"
#include "driver/adc.h"

#define OSCIL_I2S_NUM I2S_NUM_0

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

    esp_err_t readAdc_Continue(uint16_t *buffer, uint16_t *readed) override
    {
        size_t bytesRead = 0;
        return i2s_read(OSCIL_I2S_NUM, buffer, sizeof(uint16_t) * _bufferSize, (size_t *)readed, portMAX_DELAY);
    }

    esp_err_t initAdc_Continue(uint16_t bufferSize, uint sampleRate) override
    {
        MainBoard::initAdc_Continue(bufferSize, sampleRate);

        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
            .sample_rate = _sampleRate,                   // The format of the signal using ADC_BUILT_IN
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
            .channel_format = I2S_CHANNEL_FMT_ALL_LEFT,
            .communication_format = i2s_comm_format_t::I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,  // number of DMA buffers
            .dma_buf_len = 1024, // number of samples (in bytes)
            .use_apll = false,   // no Audio PLL
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0};

        i2s_driver_install(OSCIL_I2S_NUM, &i2s_config, 0, NULL);
        // delay(500);
        i2s_set_adc_mode(getAdcInfo().unit, getAdcInfo().chanelAdc1);

        initAdc_SingleRead();

        logi::p("Esp32Board", "I2s init ok");

        // SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
        // SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table

        // delay(300); // required for stability of ADC (вроде и без задержек работает норм, но оставлю как напоминание)

        auto i2s_adc_err = i2s_adc_enable(OSCIL_I2S_NUM);
        logi::err("Oscil i2s", i2s_adc_err);

        // delay(200); // required for stability of ADC

        return i2s_adc_err;
    }

    esp_err_t deinitAdc_Continue() override
    {
        logi::err("Esp32Board", i2s_adc_disable(OSCIL_I2S_NUM));
        return i2s_driver_uninstall(OSCIL_I2S_NUM);
    }

    esp_err_t changeSampleRate(uint sampleRate)
    {
        Esp32Virtual::changeSampleRate(sampleRate);
        return i2s_set_sample_rates(OSCIL_I2S_NUM, sampleRate);
    }

    uint16_t getPwmPin() override
    {
        return DAC1;
    }
};