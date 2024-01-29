/*
    Тут обозначены общие для esp32 моменты, структуры и перчисления, что бы не повторять и в классе каждой платы
*/

#include "board_virtual.h"
#include "displays/display_virtual.h"
#include "controls/control_virtual.h"

#define GET_UNIT(x)        ((x>>3) & 0x1)

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

class Esp32Virtual : public MainBoard
{
protected:
    // Хранение характеристик ADC
    esp_adc_cal_characteristics_t *_adc_chars;
    adc_calibration_type _calibrationType;

public:
    Esp32Virtual(DisplayVirtual *display, ControlVirtual *control) : MainBoard(display, control)
    {
    }

    virtual esp_adc_cal_characteristics_t *getAdcChars()
    {
        return _adc_chars;
    }

    virtual init_adc_info getAdcInfo() = 0;

    uint16_t readAdc_Single()
    {
        return (uint16_t)adc1_get_raw(getAdcInfo().chanelAdc1);
    }

    esp_err_t initAdc_SingleRead() override
    {
        // Подготавливаем переменную для характеристик АЦП для последующих преобразований
        _adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

        // Конфигурация и настройка АЦП(ADC)
        // Указываем разрядность, канал и аттенюацию (ADC_ATTEN_DB_11 должен уменьшать макс напряжение до 2.5v)
        adc1_config_width(getAdcInfo().width);
        adc1_config_channel_atten(getAdcInfo().chanelAdc1, getAdcInfo().atten);

        esp_adc_cal_value_t val_type = esp_adc_cal_characterize(getAdcInfo().unit, getAdcInfo().atten, getAdcInfo().width,
                                                                ESP_ADC_CAL_VAL_DEFAULT_VREF, _adc_chars);

        if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
        {
            Serial.println("Config adc OK");
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

        return ESP_OK;
    }

    esp_err_t deinitAdc_SingleRead() override
    {
        return ESP_OK;
    }
};