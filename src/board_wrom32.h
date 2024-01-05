#pragma once
#include <Arduino.h>

//esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "board_virtual.h"

init_adc_info adcInfo = {.unit = ADC_UNIT_1, .chanelAdc1 = ADC1_CHANNEL_0, .atten = ADC_ATTEN_11db,  .width = ADC_WIDTH_12Bit, };
MainBoard mainBoard(adcInfo);


// void board_init(esp_adc_cal_characteristics_t *adc_chars){

//   // Конфигурация и настройка АЦП(ADC)
//   // Указываем разрядность, канал и аттенюацию (ADC_ATTEN_DB_11 должен уменьшать макс напряжение до 2.5v)
//   adc1_config_width(ADC_WIDTH_12Bit);
//   adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  
//   esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, ESP_ADC_CAL_VAL_DEFAULT_VREF, adc_chars);
   
//     if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
//         Serial.println("eFuse Vref");
//     } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
//         Serial.println("Two Point");
//     } else {
//         Serial.println("Default");
//     }
// }
