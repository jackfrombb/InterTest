#include <Arduino.h>

// функция для перевода xTicksToDelay в микросекунды
uint32_t ticks_to_microseconds(TickType_t xTicksToDelay)
{
    // получение периода тика в миллисекундах
    uint32_t tick_period_ms = portTICK_PERIOD_MS;

    // перевод периода тика в микросекунды
    uint32_t tick_period_us = tick_period_ms * 1000;

    // умножение количества тиков на период тика в микросекундах
    uint32_t delay_us = xTicksToDelay * tick_period_us;

    // возвращение результата в микросекундах
    return delay_us;
}

// функция для перевода микросекунд в xTicksToDelay
TickType_t microseconds_to_ticks(uint32_t delay_us) {
  // получение периода тика в миллисекундах
  uint32_t tick_period_ms = portTICK_PERIOD_MS;

  // перевод периода тика в микросекунды
  uint32_t tick_period_us = tick_period_ms * 1000;

  // деление задержки в микросекундах на период тика в микросекундах
  TickType_t xTicksToDelay = delay_us / tick_period_us;

  // возвращение результата в тиках
  return xTicksToDelay;
}

