#pragma once

#include <Arduino.h>
// #include "board_virtual.h"
// #include "oscil_virtual.h"
// #include "hard_timer.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include <string.h>
#include <stdio.h>
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
// #include "logi.h"

// Размер буфера для хранения результатов АЦП (должен быть больше экрана, для синхронизации периодических сигналов)
#define ADC_BUFFER_SIZE 512

/// @brief Общая логика снятия осцилограммы с АЦП в continue режиме
class OscilLogic : public OscilVirtual
{
private:
    AdcVirtual *_adc = nullptr;

    setting_args_int_range _freqSetting = setting_args_int_range(0, "lastSampleData", 0, 20000000, 100000);

    //uint _sampleRate = 1000;

    // Буферы для хранения результатов АЦП
    uint8_t adc_buffer[ADC_BUFFER_SIZE] = {0};      // Буфер для байтов замеров
    uint16_t adc_buffer_out[ADC_BUFFER_SIZE] = {0}; // Буфер для полуения показаний от 0-4096
    uint32_t read_len = 0;                          // Размер прочитаных байт

    // Для управления потоком по получению данных с ацп
    TaskHandle_t _workingThreadHandler;

    // Последнее время запуска (для синхронизации считываний по времени)
    TickType_t xLastWakeTime;

    // xFrequency - луше если кратно выводу кадров на экран (50)
    // Чем меньше считываний в буфер тем удобнее смотреть сигнал (без синхронизации), но выглядит тормознутее
    const TickType_t xFrequency = 100;

    // Флаг паузы (пропуска заполнения буфера)
    bool _pause = false; // Флаг паузы
    // bool _taskIsRuning = false; //Флаг запуска задачи считывания с ацп

    /// @brief Считывание
    /// @param pvParameters
    static IRAM_ATTR void readSignal(void *pvParameters)
    {
        OscilLogic *oscil = (OscilLogic *)pvParameters;
        oscil->xLastWakeTime = xTaskGetTickCount();
        // oscil->_taskIsRuning = true;

        size_t read_len = 0;

        while (true)
        {
            if (!oscil->_pause && !oscil->_bufferBussy)
            {
                oscil->_bufferBussy = true;
                oscil->_adc->readData(oscil->adc_buffer_out, &read_len);
                oscil->read_len = (uint32_t)read_len;
                oscil->_bufferBussy = false;

                // Serial.println(String(oscil->read_len));
            }

            vTaskDelayUntil(&oscil->xLastWakeTime, oscil->xFrequency);
        }
    }

    void _startThread()
    {
        xTaskCreatePinnedToCore(
            readSignal,             // Function to implement the task
            "readSignal",           // Name of the task
            4096,                   // Stack size in bytes
            this,                   // Task input parameter
            1000,                   // Priority of the task
            &_workingThreadHandler, // Task handle.
            0                       // Core where the task should run
        );
    }

public:
    OscilLogic(MainBoard *mainBoard) : OscilVirtual(mainBoard)
    {
        //_sampleRate = sampleRate;
        _adc = mainBoard->getAdcContinue();
    }
    ~OscilLogic() override
    {
        deinit();
        _mainBoard->removeAdcContinue();
    }

    void readNext() override
    {
        bool _bufferReady = false;
    }

    uint16_t *getBuffer() override { return adc_buffer_out; }

    uint16_t getReadedLength() { return read_len; }

    bool isBufferReady() override
    {
        return _bufferReady;
    }

    esp_err_t init() override
    {
        auto ret = _adc->init(ADC_BUFFER_SIZE, _freqSetting.currentVal);
        if (logi::err("OscilLogic", ret))
        {
            _startThread();
        }
        return ESP_OK;
    }

    void deinit() override
    {
        _pause = true;
        _adc->deinit();
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
        return _adc->getSampleRate();
    }

    void setMeasuresInSecond(uint32_t tickTime) override
    {
        _pause = true;
        tickTime = range(tickTime, _adc->getMinAdcSampleRate(), _adc->getMaxAdcSampleRate()); // Проверка, что значение входит дипазон допустимых
        auto err = _adc->changeSampleRate(tickTime);
        _pause = false;


        if (logi::err("OscilContinue", err)) // Если успешно то сохраняем семплрейт
        {
            _freqSetting.currentVal = tickTime;
            _freqSetting.saveToRom();
            //AppData::saveUInt("lastSampleData", tickTime);
        }
    }

    uint16_t getBufferLength() override
    {
        return ADC_BUFFER_SIZE;
    }
};