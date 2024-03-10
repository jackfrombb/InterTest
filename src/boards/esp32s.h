#pragma once
/*
 * Полностью повторяет esp32 wroom32. Сделал отдельно, на случай если придется менять пины
 */

#include "esp32_wroom32.h"
#include "logi.h"
#include "helpers.h"

#define ADC_I2S_MODE // i2s для wroom32 показывает меньше шумов и вообще лучше работает по ощущениям
// #define ADC_DMA_MODE

// Временный костыль переключающий доступные режимы ацп
// В планах или оставить один, или сделать переключение в настройках, для тех плат где оно доступно
#ifdef ADC_DMA_MODE
#include "controllers/esp32_adc_dma.h"
#elif defined ADC_I2S_MODE
#include "controllers/esp32_adc_i2s.h"
#endif

class Esp32S : public Esp32Wroom32
{
};