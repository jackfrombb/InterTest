#pragma once
#include <Arduino.h>

class OscilVirtual
{
    protected:
    bool _bufferReady = false; // Флаг заполненности  буфера

private:
    /* data */
public:
    OscilVirtual(/* args */){}
    ~OscilVirtual(){}

    virtual void readNext(){
        bool _bufferReady = false; 
    }

    virtual ulong getRealSampleTime() {return 0;}
    virtual uint16_t *getBuffer() {return 0;}

    virtual bool isBufferReady() {
        return _bufferReady;
    }
    virtual esp_err_t init()= 0;
    virtual void deinit() = 0;
    virtual bool playPause() = 0;
    virtual uint32_t getMeasuresInSecond() {return 0;};
    virtual void setMeasuresInSecond(uint32_t tickTime) {}
    virtual uint16_t getBufferLength() = 0;
};
