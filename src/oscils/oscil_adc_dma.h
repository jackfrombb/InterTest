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

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 1
// Размер буфера для хранения результатов АЦП
#define ADC_BUFFER_SIZE 512
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Частота преобразований АЦП в Гц
#define ADC_FREQUENCY 98000
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 4096
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

class OscilAdcDma : public OscilVirtual
{
private:
    // Массив каналов АЦП
    adc_channel_t adc_channels[ADC_CHANNEL_NUM] = {ADC_CHANNEL_0};
    // Маска каналов АЦП
    uint16_t adc_channel_mask = BIT(0);
    // Буфер для хранения результатов АЦП
    uint8_t adc_buffer[ADC_BUFFER_SIZE];
    uint16_t adc_buffer_out[ADC_BUFFER_SIZE] = {};

    // uint8_t _buffer[OSCIL_ADC_DMA_BUFFER_LENGTH] = {0};
    // MainBoard *_mainBoard;
    // adc_digi_init_config_t _init_dig_cfg;
    // adc_digi_configuration_t dig_cfg;
    // adc_digi_pattern_config_t adc_pattern;
    int _sampleRate;

    TaskHandle_t _workingThread;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 100;

    static void readSignal(void *pvParameters)
    {
        OscilAdcDma *oscil = (OscilAdcDma *)pvParameters;
        // uint32_t readed = 0;
        oscil->xLastWakeTime = xTaskGetTickCount();

            // Читаем данные из пула памяти в буфер
            size_t read_len = 0;

        while (true)
        {
            // Ждем, пока пул памяти заполнится
            adc_digi_read_bytes(oscil->adc_buffer, ADC_BUFFER_SIZE, &read_len, ADC_MAX_DELAY);

            for (int i = 0; i < read_len; i+=2)
            {
                oscil->adc_buffer_out[i/2] = (oscil->adc_buffer[i+1]<<8) | oscil->adc_buffer[i];
                //logi::p("OScil", "Raw: " + String(oscil->adc_buffer_out[i/2]));
            }

            // adc_digi_read_bytes(oscil->_buffer, OSCIL_ADC_DMA_BUFFER_LENGTH, &readed, ADC_MAX_DELAY);
            // // Serial.println("Readed: " + String(readed));
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
    virtual uint16_t *getBuffer() { return (uint16_t *)adc_buffer_out; }
    bool isBufferReady()
    {
        return _bufferReady;
    }

    virtual esp_err_t init()
    {
        _mainBoard->adc1Init();
        // _init_dig_cfg = {
        //     .max_store_buf_size = 1024,
        //     .conv_num_each_intr = OSCIL_ADC_DMA_BUFFER_LENGTH,
        //     .adc1_chan_mask = BIT(0) | BIT(1),
        //     .adc2_chan_mask = BIT(0),
        // };

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
            .sample_freq_hz = (uint32_t)ADC_FREQUENCY,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };
        // adc_digi_initialize(&_init_dig_cfg);

        // adc_digi_controller_configure(&con);
        // adc_digi_start();

        // Настраиваем параметры АЦП
        adc_digi_init_config_t adc_config = {
            .max_store_buf_size = ADC_POOL_SIZE,   // максимальный размер пула памяти для DMA
            .conv_num_each_intr = ADC_CONVERSIONS, // количество преобразований в одном прерывании
            .adc1_chan_mask = adc_channel_mask,    // маска каналов АЦП
            .adc2_chan_mask = 0,                   // не используем АЦП2
        };
        // Создаем дескриптор АЦП
        adc_digi_initialize(&adc_config);

        // // Настраиваем параметры преобразований
        // adc_digi_configuration_t dig_config = {
        //     .conv_limit_en = 1,                     // включаем ограничение количества преобразований
        //     .conv_limit_num = ADC_CONVERSIONS,      // устанавливаем лимит преобразований
        //     .sample_freq_hz = ADC_FREQUENCY,        // устанавливаем частоту преобразований
        //     .conv_mode = ADC_CONV_SINGLE_UNIT_1,    // используем только АЦП1
        //     .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1, // используем формат вывода типа 1
        // };

        // // Настраиваем шаблон преобразований
        // adc_digi_pattern_config_t adc_pattern[ADC_CHANNEL_NUM] = {0};
        // dig_config.pattern_num = ADC_CHANNEL_NUM; // устанавливаем количество каналов в шаблоне

        // for (int i = 0; i < ADC_CHANNEL_NUM; i++)
        // {
        //     adc_pattern[i].atten = ADC_ATTEN_DB_11;         // устанавливаем коэффициент ослабления 11 дБ
        //     adc_pattern[i].channel = adc_channels[i] & 0x7; // устанавливаем номер канала
        //     adc_pattern[i].unit = ADC_UNIT_1;               // устанавливаем номер АЦП
        //     adc_pattern[i].bit_width = ADC_WIDTH_BIT_12;    // устанавливаем разрешение 12 бит
        // }
        // dig_config.adc_pattern = adc_pattern; // присваиваем шаблон преобразований
                                              // Применяем настройки преобразований

                                              
        adc_digi_controller_configure(&con);

        // Запускаем преобразования АЦП
        adc_digi_start();

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
        return ADC_BUFFER_SIZE;
    }
};