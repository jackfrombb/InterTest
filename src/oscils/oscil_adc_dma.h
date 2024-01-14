#pragma once

#include <Arduino.h>
#include "board_virtual.h"
#include "oscil_virtual.h"
#include "hard_timer.h"
#include "freertos/FreeRTOS.h"
#include <string.h>
#include <stdio.h>
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "driver/adc_common.h"
// #include "driver/adc_deprecated.h"

#ifndef OSCIL_ADC_DMA_BUFFER_LENGTH
#define OSCIL_ADC_DMA_BUFFER_LENGTH 512
#endif

class OscilAdcDma : public OscilVirtual
{
private:
    uint8_t _buffer[OSCIL_ADC_DMA_BUFFER_LENGTH] = {0};
    MainBoard *_mainBoard;
    adc_digi_init_config_t _init_dig_cfg;
    adc_digi_configuration_t dig_cfg;
    adc_digi_pattern_config_t adc_pattern;
    int _sampleRate;
    HardTimer _osilTimer;
    TaskHandle_t _workingThread;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 100;

    static void readSignal(void *pvParameters)
    {
        OscilAdcDma *oscil = (OscilAdcDma *)pvParameters;
        uint32_t readed = 0;
        oscil->xLastWakeTime = xTaskGetTickCount();

        while (true)
        {
            adc_digi_read_bytes(oscil->_buffer, OSCIL_ADC_DMA_BUFFER_LENGTH, &readed, ADC_MAX_DELAY);
            //Serial.println("Readed: " + String(readed));
            vTaskDelayUntil(&oscil->xLastWakeTime, oscil->xFrequency);
        }
    }

public:
    OscilAdcDma(MainBoard *mainBoard, int sampleRate) : OscilVirtual(mainBoard)
    {
        _sampleRate = sampleRate;
    }
    ~OscilAdcDma()
    {
    }

    virtual void readNext()
    {
        bool _bufferReady = false;
    }

    virtual ulong getRealSampleTime() { return 0; }
    virtual uint16_t *getBuffer() { return (uint16_t *)_buffer; }
    bool isBufferReady()
    {
        return _bufferReady;
    }

    virtual esp_err_t init()
    {
        _init_dig_cfg = {
            .max_store_buf_size = 1024,
            .conv_num_each_intr = OSCIL_ADC_DMA_BUFFER_LENGTH,
            .adc1_chan_mask = BIT(0) | BIT(1),
            .adc2_chan_mask = BIT(0),
        };

        // static adc_digi_init_config_s initConf{
        //     .max_store_buf_size = 1024,
        //     .conv_num_each_intr = 256,
        //     .adc1_chan_mask = BIT(0) | BIT(1),
        //     .adc2_chan_mask = BIT(0),
        // };

        static adc_digi_pattern_config_t adc_patern = {
            .atten = ADC_11db,
            .channel = ADC1_CHANNEL_0,
            .unit = ADC_UNIT_1,
            .bit_width = ADC_WIDTH_BIT_12,
        };

        static adc_digi_configuration_t con{
            .conv_limit_en = true,
            .conv_limit_num = 255,
            .adc_pattern = &adc_patern,
            .sample_freq_hz = (uint32_t)_sampleRate,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };
        adc_digi_initialize(&_init_dig_cfg);

        adc_digi_controller_configure(&con);
        adc_digi_start();

        // _osilTimer = HardTimer(timerInterrupt, TIMER_GROUP_0, TIMER_1, 10000, 2);
        // _osilTimer.setArgs(this);
        // _osilTimer.init();
        // Прикрепить процесс к ядру

        xTaskCreatePinnedToCore(
            readSignal,      // Function to implement the task
            "readSignal",    // Name of the task
            4084,            // Stack size in bytes
            this,            // Task input parameter
            1000,            // Priority of the task
            &_workingThread, // Task handle.
            0                // Core where the task should run
        );

        return ESP_OK;
    }

    virtual void deinit()
    {
        adc_digi_stop();
    }
    virtual bool playPause()
    {
        return true;
    }
    virtual uint32_t getMeasuresInSecond()
    {
        return 0;
    }
    virtual void setMeasuresInSecond(uint32_t tickTime)
    {
    }

    virtual uint16_t getBufferLength()
    {
        return OSCIL_ADC_DMA_BUFFER_LENGTH;
    }
};