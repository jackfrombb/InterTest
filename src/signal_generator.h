#pragma once
#include <Arduino.h>

typedef enum {
    SIGNAL_TYPE_NONE, SIGNAL_TYPE_MEANDR, SIGNAL_TYPE_WAVE, SIGNAL_TYPE_SAWTOOTH
} signal_type;

class SignalGenerator
{
private:
    uint8_t _pin;             // Пин вывода, для поддержки аналоговых сигналов должен быть dac выводом
    signal_type _currentMode = SIGNAL_TYPE_NONE; //Тип генерируемого сигнала
    uint32_t _pwmFreq = 1000; // max 150khz = 150000
    float _duty = 0.5;        // 0.0-1.0 = 0-100%

public:
    SignalGenerator(uint8_t dacPin)
    {
        _pin = dacPin;
    }

    ~SignalGenerator() = default;

    void startMenadr(uint32_t frq, float duty)
    {

    }

    void startWave(float frq)
    {

    }

    void startSawtooth(float frq)
    {

    }
};