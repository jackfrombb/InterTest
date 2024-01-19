#pragma once
#include <Arduino.h>
#include <limits>

#ifndef GENERATOR_LEDC_PWM_NUM
#define GENERATOR_LEDC_PWM_NUM 2
#endif

typedef enum
{
    SIGNAL_TYPE_NONE,
    SIGNAL_TYPE_MEANDR_LEDC,
    SIGNAL_TYPE_MEANDR_DAC,
    SIGNAL_TYPE_WAVE,
    SIGNAL_TYPE_SAWTOOTH
} signal_type;

class SignalGenerator
{
private:
    uint8_t _pin;                                // Пин вывода, для поддержки аналоговых сигналов должен быть dac выводом
    signal_type _currentMode = SIGNAL_TYPE_NONE; // Тип генерируемого сигнала
    uint32_t _pwmFreq = 1000;                    // max 150khz = 150000
    float _duty = 0.5;                           // 0.0-1.0 = 0-100%

public:
    SignalGenerator(uint8_t dacPin)
    {
        _pin = dacPin;
    }

    ~SignalGenerator() = default;

    void startMeandrLedc(uint32_t frq, float duty)
    {
        // Номер шим, частота, битность (определяет максимальное число скважности)
        ledcSetup(GENERATOR_LEDC_PWM_NUM, frq, 8);
        // Номер пина вывода, номер шим
        ledcAttachPin(_pin, GENERATOR_LEDC_PWM_NUM);
        // Номер шим, скважность
        ledcWrite(GENERATOR_LEDC_PWM_NUM, ((float)std::numeric_limits<uint8_t>::max()) * duty);

        _currentMode = signal_type::SIGNAL_TYPE_MEANDR_LEDC;
    }

    void startMenadrDac(uint32_t frq, float duty)
    {
    }

    void startWaveDac(float frq)
    {
    }

    void startSawtoothDac(float frq)
    {
    }

    void stop()
    {
        switch (_currentMode)
        {
        case signal_type::SIGNAL_TYPE_MEANDR_DAC:
            ledcDetachPin(_pin);
            break;
        }
    }
};