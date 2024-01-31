#pragma once

#include "esp32_virtual.h"
#include "logi.h"
#include "helpers.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "soc/syscon_periph.h"

// #define ADC_I2S_MODE
#define ADC_DMA_MODE

#define OSCIL_I2S_NUM I2S_NUM_0

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 1
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 1024
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

class Esp32Wroom32 : public Esp32Virtual
{
private:
    const init_adc_info _adcInfo = {
        .unit = ADC_UNIT_1,
        .chanelAdc1 = ADC1_CHANNEL_0,
        .atten = ADC_ATTEN_11db,
        .width = ADC_WIDTH_12Bit,
    };

#ifdef ADC_I2S_MODE
    /// @brief Инициализация АЦП в i2s режиме
    /// @return Ошибка или OK
    esp_err_t _initAdc_I2s()
    {
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

        SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
        SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table
                                              // REG_SET_BIT(SYSCON_SARADC_CTRL_REG, SYSCON_SARADC_DATA_TO_I2S);

        // delay(300); // required for stability of ADC (вроде и без задержек работает норм, но оставлю как напоминание)

        auto i2s_adc_err = i2s_adc_enable(OSCIL_I2S_NUM);
        logi::err("Oscil i2s", i2s_adc_err);

        // delay(200); // required for stability of ADC
    }

#elif defined(ADC_DMA_MODE)

    uint32_t adc1_channel_mask = BIT(0);
    uint32_t adc2_channel_mask = 0;
    adc1_channel_t channel[1] = {
        ADC1_CHANNEL_0,
    };

    uint8_t *buffer8bit;
    // Для отображения семплов в секунду
    uint16_t _outBufferSize = 512;

    static bool _check_valid_data(const adc_digi_output_data_t *data)
    {
        if (data->type1.channel >= SOC_ADC_CHANNEL_NUM(ADC_UNIT_1))
        {
            return false;
        }
        return true;
    }

    /// @brief Инициализация АЦП в DMA режиме, что бы освобоить i2s для других нужд (генирации к примеру)
    /// @return Ошибка ли ОК
    esp_err_t _initAdc_Dma()
    {
        buffer8bit = (uint8_t *)calloc(_bufferSize, sizeof(uint8_t));

        adc_digi_init_config_t adc_config = adc_digi_init_config_t{
            .max_store_buf_size = (uint32_t)_bufferSize << 1,
            .conv_num_each_intr = (uint32_t)_bufferSize,
            .adc1_chan_mask = adc1_channel_mask,
            .adc2_chan_mask = adc2_channel_mask,
        };

        if (logi::err("Esp32Board - initAdcDigi", adc_digi_initialize(&adc_config)))
        {
            logi::p("Esp32Board", "Init adc digi OK");
        }

        adc_digi_configuration_t con = adc_digi_configuration_t{
            .conv_limit_en = 1, // Для обычных esp32 это всегда true
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

#elif defined
#endif

public:
    Esp32Wroom32(DisplayVirtual *display, ControlVirtual *control) : Esp32Virtual(display, control)
    {
    }

    init_adc_info getAdcInfo() override
    {
        return _adcInfo;
    }

    esp_err_t readAdc_Continue(uint16_t *buffer, size_t *readLenght) override
    {
#ifdef ADC_I2S_MODE
        size_t bytesRead = 0;
        return i2s_read(OSCIL_I2S_NUM, buffer, sizeof(uint16_t) * _bufferSize, (size_t *)readLenght, portMAX_DELAY);
#elif defined(ADC_DMA_MODE)

        auto retErr = adc_digi_read_bytes(buffer8bit, _bufferSize, readLenght, ADC_MAX_DELAY);

        for (int i = 0; i < *readLenght; i += ADC_RESULT_SIZE)
        {
            adc_digi_output_data_t *p = (adc_digi_output_data_t *)&buffer8bit[i];
            if (_check_valid_data(p))
            {
                buffer[i >> 1] = p->type1.data;
            }
        }

        return ESP_OK;
#endif
    }

    esp_err_t initAdc_Continue(uint16_t bufferSize, uint sampleRate) override
    {
#ifdef ADC_I2S_MODE
        MainBoard::initAdc_Continue(bufferSize, sampleRate);
        return _initAdc_I2s();
#elif defined(ADC_DMA_MODE)
        _sampleRate = sampleRate;
        _outBufferSize = bufferSize;
        _bufferSize = bufferSize * 2;

        initAdc_SingleRead();

        return _initAdc_Dma();
#endif
    }

    esp_err_t deinitAdc_Continue() override
    {
#ifdef ADC_I2S_MODE
        logi::err("Esp32Board", i2s_adc_disable(OSCIL_I2S_NUM));
        return i2s_driver_uninstall(OSCIL_I2S_NUM);
#elif defined(ADC_DMA_MODE)
        free(buffer8bit);
        logi::err("Esp32Board - adc digi stop", adc_digi_stop());
        return logi::err("Esp32Board - adc digi deinit", adc_digi_deinitialize());
#endif
    }

    esp_err_t changeSampleRate(uint sampleRate)
    {
#ifdef ADC_I2S_MODE
        Esp32Virtual::changeSampleRate(sampleRate);
        return i2s_set_sample_rates(OSCIL_I2S_NUM, sampleRate);
#elif defined(ADC_DMA_MODE)
        deinitAdc_Continue();
        Esp32Virtual::changeSampleRate(sampleRate);
        return initAdc_Continue(_outBufferSize, sampleRate);
#endif
    }

    uint16_t getPwmPin() override
    {
        return DAC1;
    }
};