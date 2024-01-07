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
    bool isBufferReady() {
        return _bufferReady;
    }
    virtual esp_err_t init() {return ESP_FAIL;}
    virtual void deinit() {}
    virtual bool playPause() {return false;}
    virtual uint32_t getMeasuresInSecond() {return 0;}
    virtual void setMeasuresInSecond(uint32_t tickTime) {}
};