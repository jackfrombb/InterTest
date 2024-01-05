#pragma once
#include <Arduino.h>
#include "board_virtual.h"
#include "oscil_virtual.h"
#include "freertos/FreeRTOS.h"

#define OSCIL_I2S_NUM I2S_NUM_0
#define OSCIL_I2S_BUFFER_LENGTH 512

class OscilI2C
{
private:
    MainBoard *_mainBoard;
    uint8_t sampleRate = 100000;

    
    size_t bytes_read;
    int32_t buffer[OSCIL_I2S_BUFFER_LENGTH];
    ulong t_start;
    ulong fft_loop_cntr = 0;
    bool bufferReady = false;


public:
    OscilI2C(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
    }

    ~OscilI2C()
    {
    }

    esp_err_t init()
    {
        Serial.println("I2s init start");
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
            .sample_rate = sampleRate,                            // The format of the signal using ADC_BUILT_IN
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
            .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
            .communication_format = I2S_COMM_FORMAT_STAND_MSB,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,  // number of DMA buffers
            .dma_buf_len = 1024, // number of samples (in bytes)
            .use_apll = false,   // no Audio PLL
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0};

        i2s_driver_install(OSCIL_I2S_NUM, &i2s_config, 0, NULL);
        i2s_set_adc_mode(_mainBoard->getAdcInfo()->unit, _mainBoard->getAdcInfo()->chanelAdc1);

        _mainBoard->adc1Init();
        Serial.println("I2s init ok");

        // SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
        // SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table
        delay(300); // required for stability of ADC

        auto i2c_adc_err = i2s_adc_enable(OSCIL_I2S_NUM);

        if (i2c_adc_err == ESP_OK)
            Serial.println("I2s adc enabled");
        else if (i2c_adc_err == ESP_ERR_INVALID_ARG)
            Serial.println("I2s adc INVALIDE ARGS");
        else if (i2c_adc_err == ESP_ERR_INVALID_STATE)
            Serial.println("I2s adc INVALIDE STATE");

        delay(100); // required for stability of ADC

        return i2c_adc_err;
    }


    void i2sReadSignal(void *pvParameters)
    {
        //(I2S port, destination adress, data size in bytes, bytes read counter, RTOS ticks to wait)
        while (1)
        {
            auto resultI2cRead = i2s_read(I2S_NUM_0, &buffer, OSCIL_I2S_BUFFER_LENGTH, &bytes_read, 15);
            bufferReady = true;
            delay(1000);
        }
    }
};
