#pragma once
#include <Arduino.h>
#include "board_virtual.h"
#include "oscil_virtual.h"
#include "freertos/FreeRTOS.h"
#include "logi.h"
#include "helpers.h"
#include "driver/i2s.h"
#include "driver/adc.h"

//#define OSCIL_I2S_NUM I2S_NUM_0
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

                //auto resultI2cRead = oscil->_mainBoard->readAdc_Continue(); //i2s_read(OSCIL_I2S_NUM, &oscil->_buffer, sizeof(uint16_t) * OSCIL_I2S_BUFFER_LENGTH,&oscil->bytes_read, portMAX_DELAY);

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
        auto errRet = _mainBoard->initAdc_Continue(BUFFER_LENGTH, _sampleRate);

        logi::p("Oscil i2s", "I2s init ok");

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

        return errRet;
    }

    void deinit()
    {
        _mainBoard->deinitAdc_Continue();
        vTaskDelete(_workingThread);
    }

    virtual uint16_t getBufferLength()
    {
        return OSCIL_I2S_BUFFER_LENGTH;
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
        return _isOnPause;
    }

    virtual uint32_t getMeasuresInSecond()
    {
        return _sampleRate;
    }

    virtual void setMeasuresInSecond(uint32_t tickTime)
    {
        _isOnPause = true;
        //i2s_set_sample_rates(OSCIL_I2S_NUM, tickTime);
        _sampleRate = tickTime;
        _isOnPause = false;
    }
};
