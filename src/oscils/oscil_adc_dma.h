#pragma once

#include <Arduino.h>
#include "board_virtual.h"
#include "oscil_virtual.h"
#include "hard_timer.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include <string.h>
#include <stdio.h>
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "logi.h"

// Размер буфера для хранения результатов АЦП (должен быть больше экрана, для синхронизации периодических сигналов)
#define ADC_BUFFER_SIZE 512

/// @brief Общая логика снятия осцилограммы с АЦП в continue режиме
class OscilAdcDma : public OscilVirtual
{
private:
    uint _sampleRate = 1000;

    // Буферы для хранения результатов АЦП
    uint8_t adc_buffer[ADC_BUFFER_SIZE] = {0};      // Буфер для байтов замеров
    uint16_t adc_buffer_out[ADC_BUFFER_SIZE] = {0}; // Буфер для полуения показаний от 0-4096

    // Для управления потоком по получению данных с ацп
    TaskHandle_t _workingThreadHandler;
    // Последнее время запуска (для синхронизации считываний по времени)
    TickType_t xLastWakeTime;
    // xFrequency - луше если кратно выводу кадров на экран (50)
    // Чем меньше считываний в буфер тем удобнее смотреть сигнал (без синхронизации), но выглядит тормознутее
    const TickType_t xFrequency = 50;
    // Флаг паузы (пропуска заполнения буфера)
    bool _pause = false;
    bool _deinit = false;
    bool _canDeletethread = false;

    /// @brief Считывание
    /// @param pvParameters
    static IRAM_ATTR void readSignal(void *pvParameters)
    {
        OscilAdcDma *oscil = (OscilAdcDma *)pvParameters;
        oscil->xLastWakeTime = xTaskGetTickCount();

        // Читаем данные из пула памяти в буфер
        uint16_t read_len = 0;

        while (true)
        {
            if (!oscil->_pause && !oscil->_bufferBussy)
            {
                oscil->_bufferBussy = true;
                oscil->_mainBoard->readAdc_Continue(oscil->adc_buffer_out, &read_len);
                oscil->_bufferBussy = false;
            }

            vTaskDelayUntil(&oscil->xLastWakeTime, oscil->xFrequency);
        }
    }

    void _startThread()
    {
        xTaskCreatePinnedToCore(
            readSignal,             // Function to implement the task
            "readSignal",           // Name of the task
            4084,                   // Stack size in bytes
            this,                   // Task input parameter
            1000,                   // Priority of the task
            &_workingThreadHandler, // Task handle.
            0                       // Core where the task should run
        );
    }

public:
    OscilAdcDma(MainBoard *mainBoard, int sampleRate) : OscilVirtual(mainBoard)
    {
        _sampleRate = sampleRate;
    }
    ~OscilAdcDma()
    {
        deinit();
    }

    void readNext() override
    {
        bool _bufferReady = false;
    }

    uint16_t *getBuffer() override { return adc_buffer_out; }

    bool isBufferReady() override
    {
        return _bufferReady;
    }

    esp_err_t init() override
    {
        auto ret = _mainBoard->initAdc_Continue(ADC_BUFFER_SIZE, _sampleRate);
        if (logi::err("OscilAdcDma", ret))
        {
            _startThread();
            // Serial.println("Thread  started");
        }
        return ESP_OK;
    }

    void deinit() override
    {
        _pause = true;
        _mainBoard->deinitAdc_Continue();
        vTaskDelete(_workingThreadHandler);
    }

    bool isOnPause()
    {
        return _pause;
    }

    bool playPause() override
    {
        _pause = !_pause;
        return _pause;
    }

    uint32_t getMeasuresInSecond() override
    {
        return _mainBoard->getSampleRate();
    }

    void setMeasuresInSecond(uint32_t tickTime) override
    {
        _pause = true;
        _mainBoard->changeSampleRate(tickTime);
        _pause = false;
    }

    uint16_t getBufferLength() override
    {
        return ADC_BUFFER_SIZE;
    }
};