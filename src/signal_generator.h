#pragma once
#include <Arduino.h>
// Библиотека для определения лимитов числовых типов
#include <limits>
// Подключаем библиотеку для работы с ЦАП
#include <driver/dac.h>
// Оболочка над стандартным таймером esp32
#include "hard_timer.h"

// Задаем смещение меандра в вольтах
#define OFFSET 0
// Задаем амплитуду меандра в вольтах
#define AMP 3.3
// Задаем значение ЦАП для смещения меандра
#define DAC_OFFSET OFFSET / AMP *DAC_MAX
// Задаем значение ЦАП для амплитуды меандра
#define DAC_AMP AMP / AMP *DAC_MAX
// Задаем значение ЦАП для максимального напряжения
#define DAC_MAX 255
// Задаем значение ЦАП для минимального напряжения
#define DAC_MIN 0

// Определяем номер PWM для генерации шим через ledc
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
    bool _isThreadStarted = false;
    volatile bool stopDACGenerate = false;

    uint8_t _pin;                                // Пин вывода, для поддержки аналоговых сигналов должен быть dac выводом
    signal_type _currentMode = SIGNAL_TYPE_NONE; // Тип генерируемого сигнала
    uint32_t _pwmFreq = 1000;                    // ledc max 150khz = 150000, dac wave min/max =
    float _duty = 0.5;                           // 0.0-1.0 = 0-100%
    TaskHandle_t _workingThreadHandler;

    int pulse = 0;
    int period = 0;

    void _generateDacMeandre()
    {
        // Вычисляем длительность импульса в микросекундах
        pulse = 1000000 / _pwmFreq * _duty;
        // Вычисляем период меандра в микросекундах
        period = 1000000 / _pwmFreq;

        // Serial.println("Pulse: " + String(pulse) + " Peiod: " + String(period));

        // // Выводим максимальное напряжение на ЦАП
        // dac_output_voltage(DAC_CHANNEL_1, DAC_OFFSET + DAC_AMP);
        // // Ждем длительность импульса
        // delayMicroseconds(pulse);
        // // Выводим минимальное напряжение на ЦАП
        // dac_output_voltage(DAC_CHANNEL_1, DAC_OFFSET + DAC_MIN);
        // // Ждем остаток периода
        // delayMicroseconds(period - pulse);
    }

    static IRAM_ATTR void _workThread(void *pvParameters)
    {
        auto gen = (SignalGenerator *)pvParameters;

        // Инициализируем ЦАП
        dac_output_enable(DAC_CHANNEL_1);

        while (true)
        {
            switch (gen->_currentMode)
            {
            case signal_type::SIGNAL_TYPE_MEANDR_DAC:
                // Проблема этого подхода, это то что невозможно не блокируя поток достаточно сколько то точно
                // выводить сигнал. Решение это выводить через i2s, в dma стиле, порционно через буфер
                
                // Выводим максимальное напряжение на ЦАП
                dac_output_voltage(DAC_CHANNEL_1, 255);
                // Ждем длительность импульса
                delayMicroseconds(gen->pulse);
                // Выводим минимальное напряжение на ЦАП
                dac_output_voltage(DAC_CHANNEL_1, 0);
                // Ждем остаток периода
                delayMicroseconds(gen->period - gen->pulse);
                vTaskDelay(1); 
                break;

            default:
                vTaskDelay(100);
            }
        }
    }

    // функция для перевода микросекунд в xTicksToDelay
    TickType_t microseconds_to_ticks(uint32_t delay_us)
    {
        // получение периода тика в миллисекундах
        uint32_t tick_period_ms = portTICK_PERIOD_MS;

        // перевод периода тика в микросекунды
        uint32_t tick_period_us = tick_period_ms * 1000;

        // деление задержки в микросекундах на период тика в микросекундах
        TickType_t xTicksToDelay = delay_us / tick_period_us;

        // возвращение результата в тиках
        return xTicksToDelay;
    }

    void _stopThread()
    {
        vTaskDelete(_workingThreadHandler);
        _isThreadStarted = false;
    }

    void _startThread()
    {
        xTaskCreatePinnedToCore(
            _workThread,            // Function to implement the task
            "workThreadDac",        // Name of the task
            1024,                   // Stack size in bytes
            this,                   // Task input parameter
            1100,                   // Priority of the task
            &_workingThreadHandler, // Task handle.
            0                       // Core where the task should run
        );

        _isThreadStarted = true;
    }

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
        _pwmFreq = frq;
        _duty = duty;
    }

    void startMenadrDac(uint32_t frq, float duty)
    {
        _currentMode = signal_type::SIGNAL_TYPE_MEANDR_DAC;
        _pwmFreq = frq;
        _duty = duty;
        // Вычисляем длительность импульса в микросекундах
        pulse = 1000000 / _pwmFreq * _duty;
        // Вычисляем период меандра в микросекундах
        period = 1000000 / _pwmFreq;

        if (!_isThreadStarted)
        {
            _startThread();
        }
    }

    /// @brief Вывести синус сигнал на вывод DAC1
    /// @param frq Range: 130(130Hz) ~ 55000(100KHz)
    void startWaveDac(uint32_t frq)
    {
        dac_output_enable(DAC_CHANNEL_1);
        dac_cw_config_t config = dac_cw_config_t{
            .en_ch = DAC_CHANNEL_1,
            .scale = dac_cw_scale_t::DAC_CW_SCALE_1,
            .phase = dac_cw_phase_t::DAC_CW_PHASE_180,
            .freq = frq,
            .offset = (int8_t)-1,
        };
        dac_cw_generator_config(&config);
        dac_cw_generator_enable();

        _currentMode = signal_type::SIGNAL_TYPE_WAVE;
        _pwmFreq = frq;
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
        case signal_type::SIGNAL_TYPE_WAVE:
            dac_cw_generator_disable();
            break;
        }
    }
};