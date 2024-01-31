#pragma once

/**
 * @file esp32_adc_dma.h
 * @author jackfrombb (jack@boringbar.ru)
 * @brief Считывание показаний встроенных АЦП через DMA в continue режиме для плат esp32
 * (Проверено: wroom32u, )
 * @version 0.1
 * @date 2024-02-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "adc_virtual.h"

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 1
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 1024
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2

class Esp32AdcContinue : public AdcVirtual
{
private:

public:
    Esp32AdcContinue(bool enableConLim)
    {
    }

    int8_t init() override
    {
    }
    int8_t deinit() override
    {

    }
    virtual int8_t readData(uint16_t *buffer, uint16_t *readedLength) override
    {
    }
};