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

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 1
// Размер буфера для хранения результатов АЦП
#define ADC_BUFFER_SIZE 512
// Размер буфера для возврата
#define ADC_OUT_BUFFER_SIZE ((int)(ADC_BUFFER_SIZE * 0.5))
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Частота преобразований АЦП в Гц
#define ADC_FREQUENCY 98000
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 4096
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

/// @brief Снятие осцилограммы с помощью esp32 АЦП и DMA
class OscilAdcDma : public OscilVirtual
{
private:
    // Массив каналов АЦП
    adc_channel_t adc_channels[ADC_CHANNEL_NUM] = {ADC_CHANNEL_0};

    // Маска каналов АЦП
    uint16_t adc_channel_mask = BIT(0);

    // Буферы для хранения результатов АЦП
    uint8_t adc_buffer[ADC_BUFFER_SIZE];                        // Буфер для байтов замеров
    volatile uint16_t adc_buffer_out[ADC_OUT_BUFFER_SIZE] = {}; // Буфер для полуения показаний от 0-4096

    // Для отображения семплов в секунду
    int _sampleRate;

    // Параметры АЦП
    adc_digi_pattern_config_t adc_patern;
    adc_digi_configuration_t con;
    adc_digi_init_config_t adc_config;

    // Для управления потоком по получению данных с ацп
    TaskHandle_t _workingThreadHandler;
    // Последнее время запуска (для синхронизации считываний по времени)
    TickType_t xLastWakeTime;
    // xFrequency - луше если кратно выводу кадров на экран (50)
    // Чем меньше считываний в буфер тем удобнее смотреть сигнал (без синхронизации), но выглядит тормознутее
    const TickType_t xFrequency = 150;
    // Флаг паузы (пропуска заполнения буфера)
    bool _pause = false;

    /// @brief Считывание
    /// @param pvParameters
    static void readSignal(void *pvParameters)
    {
        OscilAdcDma *oscil = (OscilAdcDma *)pvParameters;
        oscil->xLastWakeTime = xTaskGetTickCount();

        // Читаем данные из пула памяти в буфер
        size_t read_len = 0;

        while (true)
        {
            if (!oscil->_pause)
            {
                // Ждем, пока пул памяти заполнится
                adc_digi_read_bytes(oscil->adc_buffer, ADC_BUFFER_SIZE, &read_len, ADC_MAX_DELAY);

                // if (read_len == ADC_BUFFER_SIZE)
                // {
                //     for(int i = 0; i<ADC_OUT_BUFFER_SIZE; i ++){
                //         oscil->adc_buffer_out[i] = (oscil->adc_buffer[(i<<1)+1]|oscil->adc_buffer[i<<1]);
                //     }
                // }

                for (int i = 0; i < read_len; i += 2)
                {
                    // Совмещаем байты для получения показаний
                    oscil->adc_buffer_out[(int)(i >> 1)] = (oscil->adc_buffer[i + 1] << 8) | oscil->adc_buffer[i];
                    // logi::p("OScil", "Raw: " + String(oscil->adc_buffer_out[i/2]));
                }
            }

            vTaskDelayUntil(&oscil->xLastWakeTime, oscil->xFrequency);
        }
    }

    void _initAdc()
    {

        adc_patern = {
            .atten = (unsigned char)_mainBoard->getAdcInfo().atten,
            .channel = (unsigned char)_mainBoard->getAdcInfo().chanelAdc1,
            .unit = (unsigned char)_mainBoard->getAdcInfo().unit,
            .bit_width = (unsigned char)_mainBoard->getAdcInfo().width,
        };

        con = adc_digi_configuration_t{
            .conv_limit_en = true,
            .conv_limit_num = 255,
            .adc_pattern = &adc_patern,
            .sample_freq_hz = (uint32_t)_sampleRate,
            .conv_mode = ADC_CONV_SINGLE_UNIT_1,
            .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
        };

        adc_config = {
            .max_store_buf_size = ADC_POOL_SIZE,   // максимальный размер пула памяти для DMA
            .conv_num_each_intr = ADC_CONVERSIONS, // количество преобразований в одном прерывании
            .adc1_chan_mask = adc_channel_mask,    // маска каналов АЦП
            .adc2_chan_mask = 0,                   // не используем АЦП2
        };

        // Создаем дескриптор АЦП
        adc_digi_initialize(&adc_config);

        adc_digi_controller_configure(&con);

        // Запускаем преобразования АЦП
        adc_digi_start();
    }

    void _startThread()
    {
        xTaskCreatePinnedToCore(
            readSignal,      // Function to implement the task
            "readSignal",    // Name of the task
            4084,            // Stack size in bytes
            this,            // Task input parameter
            1000,            // Priority of the task
            &_workingThreadHandler, // Task handle.
            0                // Core where the task should run
        );
    }

public:
    OscilAdcDma(MainBoard *mainBoard, int sampleRate) : OscilVirtual(mainBoard)
    {
        _sampleRate = sampleRate;
    }
    ~OscilAdcDma() = default;

    void readNext() override
    {
        bool _bufferReady = false;
    }

    ulong getRealSampleTime() override { return 0; }
    uint16_t *getBuffer() override { return (uint16_t*) adc_buffer_out; }
    bool isBufferReady() override
    {
        return _bufferReady;
    }

    esp_err_t init() override
    {
        _mainBoard->adc1Init();
        _initAdc();
        _startThread();
        return ESP_OK;
    }

    void deinit() override
    {
        adc_digi_stop();
        vTaskDelete(_workingThreadHandler);
    }

    bool playPause() override
    {
        _pause = !_pause;
        return _pause;
    }

    uint32_t getMeasuresInSecond() override
    {
        return _sampleRate;
    }

    void setMeasuresInSecond(uint32_t tickTime) override
    {
        _pause = true;
        _sampleRate = tickTime;
        con.sample_freq_hz = _sampleRate;
        adc_digi_stop();
        adc_digi_controller_configure(&con);
        adc_digi_start();
        _pause = false;
    }

    uint16_t getBufferLength() override
    {
        return ADC_BUFFER_SIZE;
    }
};