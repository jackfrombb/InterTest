/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */


#define BUFFER_LENGTH 168

#include <Arduino.h>

class oscil
{
private:
    int32_t _buffer[BUFFER_LENGTH];
    int _bufferLenght;
    bool _bufferReady = false;
    int *_measureTime;
    int _lastPos = 0;
    uint32_t (*_readValue)(void);

    ulong _interruptTime = 0;

public:
    oscil(int bufferLength, uint32_t(readValue)(void), int *measureTime)
    {
        _bufferLenght = bufferLength;
        _readValue = readValue;
    }
    // oscil(int32_t &buffer, int bufferLength, uint32_t (readValue)(void), int *measureTime);
    ~oscil() {

    }

    // прерыване для измерений
    bool IRAM_ATTR oscillTimerInterrupt(void *args)
    {
        if (_bufferReady)
        {
            delayMicroseconds(_interruptTime);
            return false;
        }
        static ulong prevInerTime = 0;
        _interruptTime = micros() - prevInerTime;

        // Измерение
        uint32_t reading = _readValue();
        _buffer[_lastPos] = reading;

        if (_lastPos == BUFFER_LENGTH)
        {
            _lastPos = 0;
            _bufferReady = true;
        }
        else
        {
            _lastPos += 1;
        }

        prevInerTime = micros();
        return false;
    }

    ulong getInterruptTime()
    {
        return _interruptTime;
    }

    

    bool isBufferReady()
    {
        return _bufferReady;
    }
};