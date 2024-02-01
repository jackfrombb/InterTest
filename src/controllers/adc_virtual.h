#pragma once

#include <Arduino.h>

#define GET_UNIT(x) ((x >> 3) & 0x1)

class AdcVirtual
{
protected:
    uint16_t _bufferSize = 512;
    uint _sampleRate = 1000;

public:
    AdcVirtual()
    {
    }
    virtual ~AdcVirtual()
    {
    }

    /// @brief Инициализация АЦП
    /// @param bufferSize Размер буфера для считанных данных
    /// @param sampleRate Число считываний в секунду
    /// @return 0 если ОК и код ошибки в стиле esp32 если ошибка
    virtual int8_t init(uint16_t bufferSize, uint sampleRate)
    {
        _bufferSize = bufferSize;
        _sampleRate = sampleRate;
        return 0;
    }

    virtual int8_t deinit()
    {
        return 0;
    }

    virtual int8_t changeSampleRate(uint sampleRate)
    {
        _sampleRate = sampleRate;
        return 0;
    }

    virtual uint getSampleRate(){
        return _sampleRate;
    }

    virtual int8_t readData(uint16_t *buffer, size_t *readedLength) = 0;
};