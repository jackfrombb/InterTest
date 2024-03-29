/*
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
*/
#pragma once

#include <Arduino.h>
#include "driver/timer.h"

struct hard_timer_info
{
    timer_group_t group;
    timer_idx_t num;
};

typedef struct
{
    void *args;
} timer_args;

class HardTimer
{
private:
    hard_timer_info _timer_info;
    int _tickTime;
    uint32_t _divider;
    timer_isr_t _handlerFunc;
    void *_args;

    /// @brief Инициализация таймера
    /// @param group Группа таймеров
    /// @param num Номер таймера
    /// @param tickTime Время запуска
    /// @param divider Делитель системной шины
    /// @param isr_handler Событие
    /// @param args Аргументы к событию
    /// @return Структура с информацией  о таймере
    static hard_timer_info initTimer(timer_group_t group, timer_idx_t num,
                                     int tickTime, uint32_t divider, timer_isr_t isr_handler, void *args)
    {
        const timer_config_t config = {
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_PAUSE,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_EN,
            .divider = divider,
        };

        timer_init(group, num, &config);
        timer_set_counter_value(group, num, 0);
        timer_set_alarm_value(group, num, tickTime);
        timer_isr_callback_add(group, num, isr_handler, args, 0);
        timer_enable_intr(group, num);
        timer_start(group, num);

        return hard_timer_info{
            .group = group,
            .num = num,
        };
    }

    /// @brief Настройка таймера для непрерывного чтения
    /// @param frequencyHz частота считывания в герцах
    /// @param clockSpeedHz частота тактирования
    void initTimerV2(uint frequencyHz, unsigned long apbClockSpeedHz = APB_CLK_FREQ)
    {
        // Расчет количества тактов таймера для заданной частоты сигнала
        double timerCountDouble = apbClockSpeedHz / std::max<uint>(frequencyHz, 1);

        // Проверка на возможность установки тактового делителя таймера
        if (timerCountDouble > 4294967295)
        {
            throw std::invalid_argument("Частота сигнала слишком низкая для данной скорости тактирования.");
        }

        // Вычисление оптимального делителя таймера
        unsigned long timerDivider = 1;
        while (timerCountDouble > 65535)
        {
            timerDivider *= 2;
            timerCountDouble /= 2;
        }

        // Настройка таймера
        timer_config_t config = {
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_PAUSE,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_EN,
            .divider = timerDivider,
        };

        timer_init(getInfo().group, getInfo().num, &config);

        // Установка начального значения счетчика и значения сигнала тревоги
        timer_set_counter_value(getInfo().group, getInfo().num, 0x00000000ULL);
        timer_set_alarm_value(getInfo().group, getInfo().num, timerCountDouble);

        // Включение прерываний
        timer_enable_intr(getInfo().group, getInfo().num);
        timer_isr_register(getInfo().group, getInfo().num, timer_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);

        // Запуск таймера
        timer_start(getInfo().group, getInfo().num);
    }

    // Обработчик прерывания таймера
    static void IRAM_ATTR timer_isr(void *para)
    {
        // Ваш код обработчика прерывания здесь
        HardTimer* timer = (HardTimer*) para;
        // Сброс флага прерывания
        //TIMERG0.int_clr_timers.t0 = 1;
        timer_group_clr_intr_status_in_isr(timer->getInfo().group, timer->getInfo().num);
        timer->_handlerFunc(timer->_args);
        // Перезагрузка таймера для следующего цикла
        //TIMERG0.hw_timer[TIMER_0].config.alarm_en = TIMER_ALARM_EN;
        timer_set_alarm(timer->getInfo().group, timer->getInfo().num, timer_alarm_t::TIMER_ALARM_EN);
    }
    
    bool _isOnPause;

public:
    HardTimer()
    {
    }

    HardTimer(bool(handlerFunc)(void *args), timer_group_t group,
              timer_idx_t num, int tickTime, uint32_t divider)
    {
        _isOnPause = true;
        _timer_info = {.group = group, .num = num};
        _tickTime = tickTime;
        _divider = divider;
        _handlerFunc = handlerFunc;
    }

    ~HardTimer()
    {
    }

    hard_timer_info init(bool(handlerFunc)(void *args), timer_group_t group, timer_idx_t num, int tickTime, uint32_t divider)
    {
        _isOnPause = true;
        _timer_info = {.group = group, .num = num};
        _tickTime = tickTime;
        _divider = divider;
        _handlerFunc = handlerFunc;

        return init();
    }

    hard_timer_info init()
    {
        _timer_info = initTimer(_timer_info.group, _timer_info.num, _tickTime, _divider, _handlerFunc, _args);
        _isOnPause = false;
        return _timer_info;
    }

    void setArgs(void *args)
    {
        _args = args;
    }

    bool isOnPause()
    {
        return _isOnPause;
    }

    bool playPause()
    {
        _isOnPause = !_isOnPause;
        if (_isOnPause)
        {
            timer_pause(_timer_info.group, _timer_info.num);
        }
        else
            timer_start(_timer_info.group, _timer_info.num);

        return _isOnPause;
    }

    void start()
    {
        timer_start(_timer_info.group, _timer_info.num);
    }

    void pause()
    {
        timer_pause(_timer_info.group, _timer_info.num);
    }

    void deinit()
    {
        timer_deinit(_timer_info.group, _timer_info.num);
    }

    /// @brief Получить время срабатывания
    /// @param  Время срабатывания (не преобразованное)
    u_int64_t getTickTime()
    {
        u_int64_t rValue;
        timer_get_alarm_value(_timer_info.group, _timer_info.num, &rValue);
        return rValue;
    }

    void setNewTickTime(uint32_t t_time)
    {
        timer_pause(_timer_info.group, _timer_info.num);
        timer_set_alarm_value(_timer_info.group, _timer_info.num, t_time);
        timer_start(_timer_info.group, _timer_info.num);
    }

    hard_timer_info getInfo()
    {
        return _timer_info;
    }
};
