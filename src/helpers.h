/* **********************************************
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */
#pragma once

#include <stdio.h>
#include <Arduino.h>
#include <cmath>

#define BIT_TOGGLE(number, bitIndex) ((number) ^= 1UL << (bitIndex));

template <typename T>
T rangeV2(T input, T min, T max)
{
  return input < min ? min : (input > max ? max : input);
}

#include <stdexcept>
#include "driver/timer.h"


/// @brief Рассчитать время запуска для непрерывного считывания
/// @param frequencyHz 
/// @return 
float calculateNextCycleTime(uint frequencyHz) {
    float nextCycleTime = 1000000.0 / frequencyHz;
    return nextCycleTime;
}


static void copy(int *src, int *dst, int len)
{
  memcpy(dst, src, sizeof(src[0]) * len);
}

/// @brief Ограничивает значение
/// @param input входное
/// @param min минимальное
/// @param max максимальное
/// @param infin если true, то при переполнении возвращает минимальное, а при "пробитии дна" максимальное
///             (используется для бесконечной прокрутки меню)
/// @return ограниченное значение
int range(int input, int min, int max, bool infin = false)
{
  return input < min ? (infin ? max : min) : (input > max ? (infin ? min : max) : input);
}

float mK = 0.4; // коэффициент фильтрации, 0.0-1.0
/// Бегущее среднее
float expRunningAverage(float newVal)
{
  static float filVal = 0;
  filVal += (newVal - filVal) * mK;
  return filVal;
}

void invertBytes(uint16_t *buf, int length)
{
  for (int i = 0; i < length; i++)
  {
    buf[i] = buf[i] & 0xFFFF;
    i++;
  }
}

int16_t invertBytes(int val)
{
  return val & 0x0FFF;
}

template <typename T>
uint16_t getMaxNumPosition(T num)
{
  return (uint16_t) floor((T)log10(num)) + 1;
}

// Обычное среднее
template <typename T>
T midArifm2(T newVal, T measureSize)
{

  static uint16_t counter = 0; // счётчик
  static float prevResult = 0; // хранит предыдущее готовое значение
  static float sum = 0;        // сумма
  sum += newVal;               // суммируем новое значение
  counter++;                   // счётчик++

  if (counter == measureSize)
  {                                 // достигли кол-ва измерений
    prevResult = sum / measureSize; // считаем среднее
    sum = 0;                        // обнуляем сумму
    counter = 0;                    // сброс счётчика

    logi::p("Helpers", "RESSET; Prev: " + String(prevResult));
  }
  logi::p("Helpers", "SUM: " + String(sum) + " Prev result: " + String(prevResult) + " Counter: " + String(counter));
  return (T)prevResult;
}
