#pragma once
#include <Arduino.h>
#include "board_virtual.h"
#include "oscil_virtual.h"
#include "freertos/FreeRTOS.h"
#include "logi.h"
#include "helpers.h"

#define OSCIL_I2S_NUM I2S_NUM_0
#define OSCIL_I2S_BUFFER_LENGTH 512

class OscilI2s : public OscilVirtual
{
private:
    uint32_t _sampleRate;
    size_t bytes_read;

    //Буфер с непреобразованными значениями
    uint16_t _buffer[OSCIL_I2S_BUFFER_LENGTH] = {};

    ulong t_start = millis();
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 50;
    ulong fft_loop_cntr = 0;
    TaskHandle_t _workingThread;
    bool _isOnPause = false;

public:
    OscilI2s(MainBoard *mainBoard, uint32_t sampleRate) : OscilVirtual(mainBoard)
    {
        _sampleRate = sampleRate;
        _bufferReady = true;
    }

    ~OscilI2s()
    {
        logi::p("I2sOscil", "Destroed");
        deinit();
    }

    ulong _prevMicros = 0;
    uint32_t _measureTime = 0;

    uint16_t adc_reading;

    static void i2sReadSignal(void *pvParameters)
    {
        OscilI2s *oscil = (OscilI2s *)pvParameters;
        logi::p("Oscil_I2S", "Start communicate");

        oscil->xLastWakeTime = xTaskGetTickCount();

        uint32_t read_counter = 0;
        uint64_t read_sum = 0;

        // uint16_t offset = (int)oscil->_mainBoard->getAdcInfo().unit * 0x1000 + 0xFFF;
        //(I2S port, destination adress, data size in bytes, bytes read counter, RTOS ticks to wait)
        while (1)
        {
            if (!oscil->_isOnPause && !oscil->_bufferBussy)
            {
                oscil->setBufferBussy(true);

                auto resultI2cRead = i2s_read(OSCIL_I2S_NUM, &oscil->_buffer, sizeof(uint16_t) * OSCIL_I2S_BUFFER_LENGTH,
                                              &oscil->bytes_read, portMAX_DELAY);

                // invertBytes(oscil->_buffer, OSCIL_I2S_BUFFER_LENGTH);

                oscil->setBufferBussy(false);
            }
            xTaskDelayUntil(&oscil->xLastWakeTime, oscil->xFrequency);
        }

        logi::p("Oscil_I2S", "End communicate");
        vTaskDelete(NULL);
    }

    esp_err_t init()
    {
        logi::p("Oscil i2s", "I2s init start");

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
        delay(500);
        i2s_set_adc_mode(_mainBoard->getAdcInfo().unit, _mainBoard->getAdcInfo().chanelAdc1);

        _mainBoard->adc1Init();

        logi::p("Oscil i2s", "I2s init ok");

        // SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
        // SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table

        delay(300); // required for stability of ADC

        auto i2s_adc_err = i2s_adc_enable(OSCIL_I2S_NUM);
        logi::err("Oscil i2s", i2s_adc_err);

        delay(100); // required for stability of ADC

        // Прикрепить процесс к ядру
        xTaskCreatePinnedToCore(
            i2sReadSignal,   // Function to implement the task
            "i2sReadSignal", // Name of the task
            1000,            // Stack size in bytes
            this,            // Task input parameter
            1000,            // Priority of the task
            &_workingThread, // Task handle.
            0                // Core where the task should run
        );

        return i2s_adc_err;
    }

    void deinit()
    {
        vTaskDelete(_workingThread);
        i2s_adc_disable(OSCIL_I2S_NUM);
        i2s_driver_uninstall(OSCIL_I2S_NUM);
    }

    virtual uint16_t getBufferLength()
    {
        return OSCIL_I2S_BUFFER_LENGTH;
    }

    ulong getRealSmapleRate()
    {
        return i2s_get_clk(OSCIL_I2S_NUM);
    }

    uint16_t *getBuffer()
    {
        return _buffer;
    }

    bool isOnPause() override
    {
        return _isOnPause;
    }

    bool playPause() override
    {
        _isOnPause = !_isOnPause;

        if (_isOnPause)
        {
            i2s_adc_disable(OSCIL_I2S_NUM);
        }
        else
        {
            i2s_adc_enable(OSCIL_I2S_NUM);
        }

        return _isOnPause;
    }

    virtual uint32_t getMeasuresInSecond()
    {
        return _sampleRate;
    }

    virtual void setMeasuresInSecond(uint32_t tickTime)
    {
        _isOnPause = true;
        i2s_set_sample_rates(OSCIL_I2S_NUM, tickTime);
        _sampleRate = tickTime;
        _isOnPause = false;
    }
};
