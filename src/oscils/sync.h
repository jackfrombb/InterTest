#pragma once

#include <Arduino.h>
#include <vector>
#include "logi.h"
using namespace std;

class SyncBuffer
{
private:
    uint16_t syncThreshold = 3000;

public:
    SyncBuffer()
    {
    }
    ~SyncBuffer() = default;

    bool isPeriodic_Trigger(uint16_t *buffer, uint32_t size)
    {
        // auto [min1, max1] = std::minmax_element(buffer[0], buffer[size / 2]);
        // auto [min2, max2] = std::minmax_element(buffer[size / 2], buffer[size - 1]);

        // if (max1 - min1 > syncThreshold && max2 - min2 > syncThreshold)
        // {
        // }

        return false;
    }

    // Функция для сравнения двух буферов и поиска смещения сигнала в них
    // Принимает два одномерных массива данных с сигналом и их размер
    // Возвращает смещение в индексах или 0, если буферы несинхронизируемые
    //template <typename T>
    static int findSignalOffset(uint16_t* buffer1, int size)
    {

        uint16_t treshold = 4096>>1;

       // logi::p("Sync", "treshold= " + String(treshold) + " Max: " + String(max) + " MIN: " + String(min));

        for (int i = 0; i < size - 2; i+=1)
        {
            // Берем текущее значение из первого буфера
            if ((buffer1[i] < treshold && buffer1[i + 2] > treshold))
            {
                logi::p("Sync", "I= " + String(i));
                return i;
            }
        }

        // Если не нашли совпадение буферов, возвращаем -1
        return 0;
    }
};