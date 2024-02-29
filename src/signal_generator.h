#pragma once
#include <Arduino.h>
// Библиотека для определения лимитов числовых типов
#include <limits>
// Подключаем библиотеку для работы с ЦАП
#include <driver/dac.h>
#include <driver/ledc.h>
#include <esp_intr_alloc.h>
// Оболочка над стандартным таймером esp32
#include "hard_timer.h"
#include <math.h>

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

// На данный момент реализован только меандр, что то другое буду делать гораздо позже и по необходимости
typedef enum
{
    SIGNAL_TYPE_NONE,
    SIGNAL_TYPE_MEANDR_LEDC,
    SIGNAL_TYPE_MEANDR_DAC,
    SIGNAL_TYPE_WAVE,
    SIGNAL_TYPE_SAWTOOTH
} signal_type;

class SignalGenerator : public iHaveShareSettings
{
private:
    bool _isEnabled = false;

    uint8_t _pin;                                // Пин вывода, для поддержки аналоговых сигналов должен быть dac выводом
    signal_type _currentMode = SIGNAL_TYPE_NONE; // Тип генерируемого сигнала

    // настройки для ledc
    ledc_mode_t mode = ledc_mode_t::LEDC_LOW_SPEED_MODE;
    ledc_timer_bit_t timer_bit = ledc_timer_bit_t::LEDC_TIMER_1_BIT;
    ledc_timer_t timer = ledc_timer_t::LEDC_TIMER_3;
    ledc_channel_t chanel = ledc_channel_t::LEDC_CHANNEL_2;

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

    function<bool(settings_args_virtual *)> _onSettingChangeEvent = [this](settings_args_virtual *args)
    {
        switch (args->id)
        {
        case 0:
            setEnable(_stateArg->currentVal);
            break;

        case 2:
        case 1:
            setFrequensy(_freqArg->currentVal);
            break;
        }

        return true;
    };

public:
    // Настройки состояния
    setting_args_bool *_stateArg = nullptr; // = setting_args_bool(0, "ge_st", true);
    ShareSetting *_stateSetting = nullptr;  // = ShareSetting(LOC_STATE, _stateArg, _onSettingChangeEvent);

    // Настройки частоты
    setting_args_int_range *_freqArg = nullptr; // = setting_args_int_range(1, "ge_frq", 0, 20000000, 100000);
    ShareSetting *_freqSetting = nullptr;       // ShareSetting(LOC_FREQ, _freqArg, _onSettingChangeEvent);

    // Настройки скважности
    setting_args_int_steep *_dutyArg = nullptr; // = setting_args_int_steep(2, "ge_dty", {}, 0);
    ShareSetting *_dutySetting = nullptr;       // ShareSetting(LOC_DUTY, _dutyArg, _onSettingChangeEvent);

protected:
    static SignalGenerator *_instance;
    vector<int> dutySteeps;

    SignalGenerator(uint8_t dacPin) : iHaveShareSettings(LOC_GENERATOR)
    {
        _pin = dacPin;

        _init();

        // Добавляем настройки в класс хранения
        addSetting(_stateSetting)
            ->addSetting(_freqSetting)
            ->addSetting(_dutySetting);
    }

    void _init()
    {
        _stateArg = new setting_args_bool(0, "ge_st", true);
        _freqArg = new setting_args_int_range(1, "ge_frq", 0, 20000000, 100000);
        setFrequensy(_freqArg->currentVal);
        uint currentSteep = (uint)(dutySteeps.size() >> 1);

        Serial.println("Steeps count : " + String(dutySteeps.size()) + " CurSteep: " + String(currentSteep));
        _dutyArg = new setting_args_int_steep(2, "ge_dty", &dutySteeps, currentSteep);

        _stateSetting = new ShareSetting(LOC_STATE, _stateArg, _onSettingChangeEvent);
        _freqSetting = new ShareSetting(LOC_FREQ, _freqArg, _onSettingChangeEvent);
        _dutySetting = new ShareSetting(LOC_DUTY, _dutyArg, _onSettingChangeEvent);

        if (_stateArg->currentVal)
        {
            startMeandrLedc();
        }
    }

public:
    /* Все аргументы настроек нужно инициализировать только после запуска и инициализации App_Data::init() в основном потоке */

    ~SignalGenerator()
    {
        delete _stateArg;
        delete _freqArg;
        delete _dutyArg;

        delete _stateSetting;
        delete _freqSetting;
        delete _dutySetting;
    }

    /// @brief Инициализация перед запуском
    /// @param dacPin
    static void init(uint8_t dacPin)
    {
        _instance = new SignalGenerator(dacPin);
    }

    /// @brief Получить объект генератора
    /// временное решение. В дальнейшем буду либо встрою в MainBoard либо ещё что то придумаю
    static SignalGenerator *get()
    {
        return _instance;
    }

    void startMeandrLedc()
    {
        // ledc_isr_register(&testLedc, this, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_HIGH, &isrHandle);

        ledc_timer_config_t ledc_timer = {
            .speed_mode = mode,
            .duty_resolution = timer_bit,
            .timer_num = timer,
            .freq_hz = (uint)_freqArg->currentVal,
            .clk_cfg = ledc_clk_cfg_t::LEDC_AUTO_CLK,
        };

        // Выставляем скважность
        uint32_t duty = (uint32_t)((pow(2.0, (float)ledc_timer.duty_resolution) - 1) * ((float)_dutyArg->getSteepValue() / 100.0)); // pow(2.0, (float)ledc_timer.duty_resolution) - 1; //((2 ** 13) - 1) - взято из примера https://github.com/espressif/esp-idf/blob/v4.4.6/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c

        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        ledc_channel_config_t config = {
            .gpio_num = _pin,
            .speed_mode = mode,
            .channel = chanel,
            .intr_type = ledc_intr_type_t::LEDC_INTR_DISABLE,
            .timer_sel = timer,
            .duty = duty,
            .hpoint = 0,
        };

        ledc_channel_config(&config);

        _currentMode = signal_type::SIGNAL_TYPE_MEANDR_LEDC;

        _isEnabled = true;
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

    void setEnable(bool enable)
    {
        if (!enable)
        {
            ledc_stop(mode, chanel, 0);
            _currentMode = signal_type::SIGNAL_TYPE_NONE;
        }
        else
        {
            _currentMode = signal_type::SIGNAL_TYPE_MEANDR_LEDC;
            startMeandrLedc();
        }
    }

    bool isGenerationEnable()
    {
        return _currentMode != signal_type::SIGNAL_TYPE_NONE;
    }

    uint32_t getFrequensy()
    {
        return (uint32_t)_freqArg->currentVal;
    }

    int getDutyCycle()
    {
        return _dutyArg->getSteepValue();
    }

    void setFrequensy(uint32_t frq)
    {
        /*
        Фнкция в тестовом режиме. Не доделана и не проверена
        нам нужно установить частоту генерации, проверить что скважность входит в диапазон
        и, если не входит, то задать подходящую частоте битность и подобрать близкую к нужной скважность

        Чем выше частота тем ниже битность и больше глючность
        */
        //Serial.println("Start set freq: " + String(frq));
        _freqArg->currentVal = range(frq, _freqArg->fromVal, _freqArg->toVal);

        if (_isEnabled)
            setEnable(false);

        dutySteeps.clear();

        if (_freqArg->currentVal > 8000000)
        {
            dutySteeps.insert(dutySteeps.end(), {50}); // 2 значения
            timer_bit = ledc_timer_bit_t::LEDC_TIMER_1_BIT;
            Serial.println("Set duty 1 bit ");
        }

        else if (_freqArg->currentVal > 5000000)
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 40, 50, 60, 80, 100}); // 4 bit 16 значений
            timer_bit = ledc_timer_bit_t::LEDC_TIMER_4_BIT;
            Serial.println("Set duty 4 bit ");
        }
        else if (_freqArg->currentVal > 300000)
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}); // 64 значений
            timer_bit = ledc_timer_bit_t::LEDC_TIMER_6_BIT;
            Serial.println("Set duty 6 bit ");
        }
        else
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}); // 256 значений
            timer_bit = ledc_timer_bit_t::LEDC_TIMER_8_BIT;
            Serial.println("Set duty 8 bit ");
        }

        if (_isEnabled)
            setEnable(true);
    }
};

SignalGenerator *SignalGenerator::_instance = nullptr;