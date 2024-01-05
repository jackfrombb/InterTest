#pragma once

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2s.h"

/// @brief Информация о конфигурации АЦП
typedef struct 
{
    adc_unit_t unit;
    adc1_channel_t chanelAdc1;
    adc_atten_t atten;
    adc_bits_width_t width;
} init_adc_info;

/// Тип калибровки показаний
enum adc_calibration_type
{
    CALIBR_EFUSE_VREF = 0,
    CALIBR_TWO_POINT = 1,
    CALIBR_NO = 2,
};

/// @brief Основная плата устройства
class MainBoard
{
protected:
    // Хранение характеристик ADC
    esp_adc_cal_characteristics_t *_adc_chars;
    init_adc_info *_adcInfo;
    adc_calibration_type _calibrationType;
    uint32_t (*_value_read_func)();

private:
public:
    MainBoard(init_adc_info adcInfo)
    {
        _adcInfo = &adcInfo;
    }

    virtual void adc1Init(){
        // Подготавливаем переменную для характеристик АЦП для последующих преобразований
        _adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

        // Конфигурация и настройка АЦП(ADC)
        // Указываем разрядность, канал и аттенюацию (ADC_ATTEN_DB_11 должен уменьшать макс напряжение до 2.5v)
        adc1_config_width(_adcInfo->width);
        adc1_config_channel_atten(_adcInfo->chanelAdc1, _adcInfo->atten);

        esp_adc_cal_value_t val_type = esp_adc_cal_characterize(_adcInfo->unit, _adcInfo->atten, _adcInfo->width,
                                                                ESP_ADC_CAL_VAL_DEFAULT_VREF, _adc_chars);

        if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
        {
            _calibrationType = CALIBR_EFUSE_VREF;
        }
        else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
        {
            _calibrationType = CALIBR_TWO_POINT;
        }
        else
        {
            _calibrationType = CALIBR_NO;
        }
    }

    virtual void init()
    {
    }

    esp_adc_cal_characteristics_t *getAdcChars()
    {
        return _adc_chars;
    }

    init_adc_info *getAdcInfo()
    {
        return _adcInfo;
    }
};