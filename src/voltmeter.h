#pragma once

#include <Arduino.h>
// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"

class Voltmetr
{
private:
    MainBoard *_mainBoard;

    OscilVirtual *_oscil;
    uint16_t *_copyOscilBuffer; // Входное. Копия буфера АЦП, что бы не задерживать считывание

    uint16_t *_outBuffer; // Выходной буфер с преобразованными значениями (в микровольтах)
    uint16_t lastMax = 0; // Выходное максимальное в буфере (микровольт)
    uint16_t lastMid = 0; // Выходное среднее в буфере (микровольт)

    /// Обычное среднее (чуть подогнанный метод с сайта Алекса Гайвера)
    template <typename T>
    static T midArifm2(T newVal, T measureSize)
    {
        static uint16_t counter = 0; // счётчик
        static float prevResult = 0; // хранит предыдущее готовое значение
        static float sum = 0;        // сумма
        sum += newVal;               // суммируем новое значение
        counter++;                   // счётчик++

        if (counter == measureSize)
        {                                   // достигли кол-ва измерений
            prevResult = sum / measureSize; // считаем среднее
            sum = 0;                        // обнуляем сумму
            counter = 0;                    // сброс счётчика

            //logi::p("Helpers", "RESSET; Prev: " + String(prevResult));
        }
        //logi::p("Helpers", "SUM: " + String(sum) + " Prev result: " + String(prevResult) + " Counter: " + String(counter));
        return (T)prevResult;
    }

public:
    Voltmetr(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
    }
    ~Voltmetr()
    {
        free(_copyOscilBuffer);
    }

    void setOscil(OscilVirtual *oscil)
    {
        _oscil = oscil;
        _copyOscilBuffer = (uint16_t *)calloc(_oscil->getBufferLength(), sizeof(uint16_t));
        _outBuffer = (uint16_t *)calloc(_oscil->getBufferLength(), sizeof(uint16_t));
    }

    float getMiddle()
    {
        return (float)lastMid / 1000; // Приводим микровольты в  вольты
    }

    float getMax()
    {
        return (float)lastMax / 1000; // Микровольты в вольты
    }

    adc_measures_t getMeasures() // Основной метод попроеобразованию, остальные значения заполняются здесь
    {
        // Ждем очереди - блокируем - копируем - освобождаем - преобразуем
        while (_oscil->isBufferBussy())
        {
            delayMicroseconds(100);
        }

        _oscil->setBufferBussy(true);
        memcpy(_copyOscilBuffer, _oscil->getBuffer(), _oscil->getBufferLength());
        _oscil->setBufferBussy(false);

        uint16_t *buffer = _copyOscilBuffer;

        uint16_t max = 0.0;
        uint16_t mid = 0.0;

        for (int i = 0; i < _oscil->getBufferLength(); i++)
        {
            auto val = _mainBoard->rawToVoltage(buffer[i]);
            _outBuffer[i] = val;

            mid = Voltmetr::midArifm2<uint16_t>(val, _oscil->getBufferLength() * 4);

            if (val > max)
            {
                max = val;
            }
        }

        lastMax = max;
        lastMid = mid;

        return adc_measures_t{
            .buffer = _outBuffer,
            .bufferSize = _oscil->getBufferLength(),
            .middle = mid,
            .max = max,
        };
    }
};