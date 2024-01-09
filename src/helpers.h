/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */
#pragma once
#include <stdio.h>
#include <Arduino.h>

extern int range(int input, int min, int max, bool infin = false);

template <typename T>
T rangeV2(T input, T min, T max)
{
  return input < min ? min : (input > max ? max : input);
}

extern void copy(int *src, int *dst, int len);

extern float mK;// = 0.4; // коэффициент фильтрации, 0.0-1.0
/// бегущее среднее
extern float expRunningAverage(float newVal);

// Обычное среднее
extern float midArifm2(float newVal, float measureSize);

/// @brief Инвертировать байты
/// @param buf ссылка на массив
/// @param length длинна массива
void invertBytes(uint16_t* buf, int length);

int16_t invertBytes(uint16_t val);