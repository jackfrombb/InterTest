/* **********************************************
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */
#pragma once

#ifndef BUFFER_LENNGH
#define BUFFER_LENGTH 168
#endif

#include <Arduino.h>
#include "hard_timer.h"
#include "board_virtual.h"
#include "oscil_virtual.h"

class OscilAdc : public OscilVirtual
{
private:
    bool _bufferReady = false; // Флаг заполненности  буфера
    uint16_t _buffer[BUFFER_LENGTH] = {}; //Буфер
    int _measureTime; //Время между измерениями
    int _lastPos = 0; //Последняя позиция записи в буфер
    int32_t _lastValue = 0; //Последнее значение записанное в буфер
    ulong _interruptTime = 0; // Фермя затраченное на одно считывание
    ulong _prevInterTime = 0; // Предыдущее время тика
    MainBoard* _board; // Информация о главной плате

public:
    HardTimer oscilTimer;
    
    OscilAdc() = default; // Для неинициализированных объектов

    OscilAdc(MainBoard* mainBoard, int measureTime)
    {
        _board = mainBoard;
        _measureTime = measureTime;
    }

    ~OscilAdc()
    = default;

    void writeToBuffer(){

        _interruptTime = micros() - _prevInterTime;

        // Измерение
        uint32_t reading = adc1_get_raw(_board->getAdcInfo().chanelAdc1); // adc1_get_raw(ADC1_CHANNEL_2);

        _buffer[_lastPos] = reading;

        if (_lastPos == BUFFER_LENGTH - 1)
        {
            _lastPos = 0;
            _bufferReady = true;
        }
        else
        {
            _lastPos += 1;
        }

        _prevInterTime = micros();
    }

    static bool IRAM_ATTR timerInterrupt(void *args)
    {
        auto *oscil = (OscilAdc *)args;
        oscil->writeToBuffer();
        return false;
    }

    ulong getRealSampleTime() override
    {
        return _interruptTime;
    }

    uint32_t getMeasuresInSecond() override{
        return getTimer().getTickTime();
    }

    void setMeasuresInSecond(uint32_t tickTime) override{
        getTimer().setNewTickTime(tickTime);
    }

    uint16_t *getBuffer() override
    {
        return _buffer;
    }
    
    uint16_t getBufferLength() override {
        return BUFFER_LENGTH;
    }

    bool isBufferReady() override
    {
        return _bufferReady;
    }

    void readNext() override
    {
        _bufferReady = false;
    }

    esp_err_t init() override
    {
        _board->adc1Init();

        oscilTimer = HardTimer(timerInterrupt, TIMER_GROUP_0, TIMER_1, _measureTime, 2);
        oscilTimer.setArgs(this);
        oscilTimer.init();

        return ESP_OK;
    }

    void deinit() override{
        oscilTimer.deinit();
    }

    bool playPause() override{
        return getTimer().playPause();
    }

    HardTimer getTimer() const
    {
        return oscilTimer;
    }
};