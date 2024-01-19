#pragma once
#include <Arduino.h>
#include "board_virtual.h"

class OscilVirtual
{
protected:
    bool _bufferReady = false; // Флаг заполненности  буфера
    bool _bufferBussy = false;
    MainBoard *_mainBoard;

private:
    /* data */
public:
    explicit OscilVirtual(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
    }

    ~OscilVirtual() {}

    virtual void readNext()
    {
        bool _bufferReady = false;
    }

    virtual ulong getRealSampleTime() { return 0; }
    virtual uint16_t *getBuffer() { return 0; }

    virtual bool isBufferReady()
    {
        return _bufferReady;
    }

    virtual esp_err_t init() = 0;
    virtual void deinit() = 0;
    virtual bool playPause() = 0;
    virtual uint32_t getMeasuresInSecond() { return 0; };
    virtual void setMeasuresInSecond(uint32_t tickTime) {}
    virtual uint16_t getBufferLength() = 0;
    virtual float getPeakToPeak()
    {
        return -1;
    }

    virtual float getMidlle()
    {
        return -1;
    }

    virtual void setBufferBussy(bool bussy)
    {
        _bufferBussy = bussy;
    }

    virtual bool isBufferBussy()
    {
        return _bufferBussy;
    }
};
