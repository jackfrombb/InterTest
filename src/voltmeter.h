#pragma once

#include <Arduino.h>
// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <vector>
#include <cmath>

class Voltmetr
{
private:
    MainBoard *_mainBoard;

    OscilVirtual *_oscil;
    uint16_t *_copyOscilBuffer; // Входное. Копия буфера АЦП, что бы не задерживать считывание

    uint16_t *_outBuffer; // Выходной буфер с преобразованными значениями (в микровольтах)
    uint16_t lastMax = 0; // Выходное максимальное в буфере (микровольт)
    uint16_t lastMid = 0; // Выходное среднее в буфере (микровольт)

    std::vector<uint16_t> peaks;

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

            // logi::p("Helpers", "RESSET; Prev: " + String(prevResult));
        }
        // logi::p("Helpers", "SUM: " + String(sum) + " Prev result: " + String(prevResult) + " Counter: " + String(counter));
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

    int16_t findPeriodV2(uint16_t *buffer, uint16_t size)
    {
        uint16_t period = 0;             // variable to store the period of the signal
        uint16_t last_value = buffer[0]; // variable to store the last value of the signal
        uint16_t threshold = 1000;        // threshold value for filtering out noise

        for (int i = 1; i < size; i++)
        {
            if (buffer[i] > last_value && buffer[i] > threshold)
            {
                period = i; // update the period
                break;
            }
            last_value = buffer[i];
        }

        return period;
    }

    int find_period(std::vector<uint16_t> signal)
    {
        std::vector<uint16_t> autocorr(signal.size() * 2 - 1);

        for (size_t i = 0; i < signal.size(); ++i)
        {
            for (size_t j = 0; j < signal.size(); ++j)
            {
                autocorr[i + j] += signal[i] * signal[j];
            }
        }

        std::vector<uint16_t>::iterator max_it = std::max_element(autocorr.begin(), autocorr.end());
        int max_index = std::distance(autocorr.begin(), max_it);

        int ret = max_index - signal.size() + 1;

        // logi::p("Voltmeter", "Max index: " + String(max_index) + " Signal size: " +
        //                          String(signal.size()) + " Period: " + String(ret));

        return ret;
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
        //std::vector<uint16_t> bufVect;

        for (int i = 0; i < _oscil->getBufferLength(); i++)
        {
            auto val = _mainBoard->rawToVoltage(buffer[i]);
            _outBuffer[i] = val;

            mid = Voltmetr::midArifm2<uint16_t>(val, _oscil->getBufferLength() * 4);
            //bufVect.push_back(val);

            if (val > max)
            {
                max = val;
            }
        }

        lastMax = max;
        lastMid = mid;

        int16_t period = findPeriodV2(_outBuffer, _oscil->getBufferLength());

        //logi::p("Voltmeter", "Period: " + String(period));

        return adc_measures_t{
            .buffer = _outBuffer,
            .bufferSize = _oscil->getBufferLength(),
            .middle = mid,
            .max = max,
            .bias = period,
        };
    }
};