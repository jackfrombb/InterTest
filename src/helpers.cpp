/* **********************************************
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */
#include <Arduino.h>

float mK = 0.4; // коэффициент фильтрации, 0.0-1.0

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
    buf[i] = buf[i] & 0x0FFF;
    i++;
  }
}

int16_t invertBytes(uint16_t val)
{
  return val & 0x0FFF;
}

// Обычное среднее
float midArifm2(float newVal, float measureSize)
{

  static byte counter = 0;     // счётчик
  static float prevResult = 0; // хранит предыдущее готовое значение
  static float sum = 0;        // сумма
  sum += newVal;               // суммируем новое значение
  counter++;                   // счётчик++
  if (counter == measureSize)
  {                                 // достигли кол-ва измерений
    prevResult = sum / measureSize; // считаем среднее
    sum = 0;                        // обнуляем сумму
    counter = 0;                    // сброс счётчика
  }
  return prevResult;
}
