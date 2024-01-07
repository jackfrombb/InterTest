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
    uint16_t _buffer[BUFFER_LENGTH]; //Буфер
    int _measureTime; //Время между измерениями
    int _lastPos = 0; //Последняя позиция записи в буфер
    int32_t _lastValue = -1; //Последнее значение записанное в буфер
    ulong _interruptTime = 0; // Фермя затраченное на одно считывание
    ulong _prevInterTime = 0; // Предыдущее время тика
    MainBoard* _board; // Информация о главной плате

public:
    HardTimer oscilTimer;
    
    OscilAdc() {} // Для неинициализированных объектов
    OscilAdc(MainBoard* mainBoard, int measureTime)
    {
        _board = mainBoard;
        _measureTime = measureTime;
    }

    ~OscilAdc()
    {
    }

    void writeToBuffer(){

        _interruptTime = micros() - _prevInterTime;

        // Измерение
        uint32_t reading = adc1_get_raw(_board->getAdcInfo()->chanelAdc1); // adc1_get_raw(ADC1_CHANNEL_2);

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
        OscilAdc *oscil = (OscilAdc *)args;
        oscil->writeToBuffer();
        return false;
    }

    ulong getRealSampleTime()
    {
        return _interruptTime;
    }

    uint32_t getMeasuresInSecond(){
        return getTimer().getTickTime();
    }

    void setMeasuresInSecond(uint32_t tickTime){
        getTimer().setNewTickTime(tickTime);
    }

    uint16_t *getBuffer()
    {
        return _buffer;
    }

    bool isBufferReady()
    {
        return _bufferReady;
    }

    void readNext()
    {
        _bufferReady = false;
    }

    esp_err_t init()
    {
        _board->adc1Init();

        oscilTimer = HardTimer(timerInterrupt, TIMER_GROUP_0, TIMER_1, _measureTime, 2);
        oscilTimer.setArgs(this);
        oscilTimer.init();

        return ESP_OK;
    }

    void deinit(){
        oscilTimer.deinit();
    }

    bool playPause(){
        return getTimer().playPause();
    }

    HardTimer getTimer()
    {
        return oscilTimer;
    }
};