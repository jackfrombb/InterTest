#pragma once

#include <Arduino.h>
#include "helpers.h"
// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"

class Voltmetr {
    private:
    esp_adc_cal_characteristics_t* _adc_chars;

    public:
    Voltmetr(){
    }
    ~Voltmetr(){
    }

    /// @brief Установить характеристики ADC. Вызывать обязательно, но после заполнения этих характеристик
    /// [esp_adc_cal_characteristics_t * adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));]
    /// Необходимы для преобразования из ADC попугаев в mV
    /// @param adc_chars Ссылка на заполненные характеристики
    void setAdcChars(esp_adc_cal_characteristics_t*  adc_chars){
        _adc_chars = adc_chars;
    }

    /// @brief Измерить пиковые значения
    /// @param buffer буфер с непреобразованными значениями
    /// @param bufferLenght длинна буфера
    /// @return напряжение в вольтах
    float measurePickVolt(int32_t* buffer, int bufferLenght){
        int32_t returnVal;
        for(int i=0 ; i<bufferLenght; i++){
            int32_t rawV = esp_adc_cal_raw_to_voltage(buffer[i], _adc_chars);
            returnVal = returnVal > rawV ? returnVal : rawV;
        }

        return returnVal/1000.0;
    }

    float measureMidVolt(int32_t* buffer, int bufferLenght){
        int32_t returnVal;
        for(int i; i<bufferLenght; i++){
            int32_t rawV = esp_adc_cal_raw_to_voltage(buffer[i], _adc_chars);
            returnVal = midArifm2(buffer[i], bufferLenght);
        }
        return returnVal/1000;
    }

};