#pragma once

#include "pwm_virtual.h"
// Библиотека для определения лимитов числовых типов
#include <limits>
// Подключаем библиотеку для работы с ЦАП
#include <driver/dac.h>
#include <driver/ledc.h>
#include <esp_intr_alloc.h>
// Оболочка над стандартным таймером esp32
#include "hard_timer.h"
#include <math.h>

class Esp32PwmController : public PwmControllerVirtual
{
    // Флаг успешного запуска генерации. Не путать с enable, поскольку enable задает пользователь, а это фактический флаг запуска
    bool _generationState = false;
    uint8_t _prevDutySteep = 0;

    // настройки для ledc
    ledc_mode_t mode = ledc_mode_t::LEDC_LOW_SPEED_MODE;
    ledc_timer_bit_t timer_bit = ledc_timer_bit_t::LEDC_TIMER_1_BIT;
    ledc_timer_t timer = ledc_timer_t::LEDC_TIMER_3;
    ledc_channel_t chanel = ledc_channel_t::LEDC_CHANNEL_2;

    uint8_t _checkDutySteeps() override
    {
        /*
            Наша задача в этом методе установить такую битность таймера, что бы заработала генерация,
            но при этом желательно сохранить скважность на таком же или близком к нему значении
         */

        // Значение скважности в 50% (положение в списке скважностей)
        uint8_t defaultVal = 0;
        // Для хранения вычисляемого таймера
        ledc_timer_bit_t newTimerBit = timer_bit;

        // Очищаем
        dutySteeps.clear();

        if (_freqArg->currentVal > 5000000)
        {
            dutySteeps.insert(dutySteeps.end(), {50}); // 2 значения
            newTimerBit = ledc_timer_bit_t::LEDC_TIMER_1_BIT;
            Serial.println("Set duty 1 bit ");
        }
        else if (_freqArg->currentVal > 1200000)
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}); // 4 bit 16 значений
            newTimerBit = ledc_timer_bit_t::LEDC_TIMER_4_BIT;
            Serial.println("Set duty 4 bit ");
            defaultVal = 4;
        }
        else if (_freqArg->currentVal > 300000)
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}); // 64 значений
            newTimerBit = ledc_timer_bit_t::LEDC_TIMER_6_BIT;
            Serial.println("Set duty 6 bit ");
            defaultVal = 4;
        }
        else
        {
            dutySteeps.insert(dutySteeps.end(), {10, 20, 30, 40, 50, 60, 70, 80, 90, 100}); // 256 значений
            newTimerBit = ledc_timer_bit_t::LEDC_TIMER_8_BIT;
            Serial.println("Set duty 8 bit ");
            defaultVal = 4;
        }

        // Если настройки скважности уже инициализированны, то значит идет настройка скважности пользователем
        // Когда таймер = 1bit то значение может быть только одно иначе применяем значение пользователя
        // Если предыдущая раязрядность 1 бит, то тоже оставляем стандартное значение
        if (_dutyArg != nullptr && newTimerBit != ledc_timer_bit_t::LEDC_TIMER_1_BIT && timer_bit != LEDC_TIMER_1_BIT)
        {
            defaultVal = _dutyArg->currentVal;
        }

        // Устанавливаем положение таймера
        timer_bit = newTimerBit;

        return defaultVal;
    }

    uint32_t _calculateDuty()
    {
        // Выставляем скважность
        return (uint32_t)((pow(2.0, (float)timer_bit) - 1) * ((float)_dutyArg->getSteepValue() / 100.0)); // pow(2.0, (float)ledc_timer.duty_resolution) - 1; //((2 ** 13) - 1) - взято из примера https://github.com/espressif/esp-idf/blob/v4.4.6/examples/peripherals/ledc/ledc_basic/main/ledc_basic_example_main.c
    }

public:
    Esp32PwmController(uint8_t dacPin) : PwmControllerVirtual(dacPin)
    {
    }

    void startMeander() override
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
        uint32_t duty = max<uint32_t>(_calculateDuty(), 1);

        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        ledc_channel_config_t config = {
            .gpio_num = _dacPin,
            .speed_mode = mode,
            .channel = chanel,
            .intr_type = ledc_intr_type_t::LEDC_INTR_DISABLE,
            .timer_sel = timer,
            .duty = duty,
            .hpoint = 0,
        };

        if (ledc_channel_config(&config))
        {
            _generationState = true;
        }
    }

    void stopMeander() override
    {
        if (_generationState)
        {
            ledc_stop(mode, chanel, 0);
            _generationState = false;
        }
    }

    bool applySetting() override
    {
        if (!isEnable())
        {
            stopMeander();
        }
        else
        {
            stopMeander();
            startMeander();
        }
        return true;
    }
};