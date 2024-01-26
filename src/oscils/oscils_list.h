#pragma once
#include "oscil_virtual.h"
#include "oscil.h"
#include "oscil_adc_dma.h"
#ifdef WROOM32
#include "oscil_i2s.h" //Только стандартные esp32 поддерживают adc через I2s
#endif