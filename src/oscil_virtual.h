#pragma once
#include <Arduino.h>

#ifndef BUFFER_LENNGH
#define BUFFER_LENGTH 168
#endif


class OscilVirtual
{
    protected:
    bool _bufferReady = false; // Флаг заполненности  буфера
    int32_t _buffer[BUFFER_LENGTH]; //Буфер

private:
    /* data */
public:
    OscilVirtual(/* args */){}
    ~OscilVirtual(){}

    virtual void readNext(){
        bool _bufferReady = false; 
    }

    
};
