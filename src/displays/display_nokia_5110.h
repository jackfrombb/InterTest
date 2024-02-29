#pragma once
#include <Arduino.h>
#include <U8g2lib.h> 
#include "u8g2_display_virtual.h"
#include <driver/ledc.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#ifdef S2MINI
// Nokia PCD8544 display
#define RST 2                       // Pin1 (RST)  GPIO2
#define CE 15                       // Pin2 (CE)  GPIO15
#define DC 4                        // Pin3 (DC)  GPIO4
#define DIN 17                      // Pin4 (Din)  GPIO17
#define CLK 18                      // Pin5 (Clk)  GPIO18
                                    // Pin6 (Vcc)  3.3V
#define DISPLAY_LED_PIN GPIO_NUM_10 // Pin7 (BL)
                                    // Pin8 (GND)  GND
#endif

#ifdef WROOM32
// Nokia PCD8544 display
#define RST GPIO_NUM_16 // Pin1 (RST)  GPIO2
#define CE GPIO_NUM_17  // Pin2 (CE)  GPIO15
#define DC GPIO_NUM_5   // Pin3 (DC)  GPIO4
#define DIN GPIO_NUM_18 // Pin4 (Din)  GPIO17
#define CLK GPIO_NUM_19 // Pin5 (Clk)  GPIO18
// #define DISPLAY_VCC GPIO_NUM_21                         // Pin6 (Vcc)  3.3V
#define DISPLAY_LED_PIN GPIO_NUM_21 // Pin7 (BL)
                                    // Pin8 (GND)  GND
#endif

/// @brief Старый дисплей от Nokia
class Nokia5110_U8g2 : public U8g2DisplayVirtual
{
protected:
    InterfaceEngineVirtual *_interfaceEngine;

    function<bool(settings_args_virtual *)> _onSettingChangeEvent = [this](settings_args_virtual *args)
    {
        switch (args->id)
        {
        case 0:
            _u8g2->setContrast(_contrastSettingsArg->currentVal);
            
            _u8g2->clearDisplay();
            _u8g2->clearBuffer();
            // logi::p("Display", "Set contrast: " + String(_contrastSettingsArg->currentVal));
            break;

        case 1:
            uint8_t val = (uint8_t)((float)255 * (_brightnessSettingsArg->currentVal / 100.0));
            ledcWrite(ledc_channel_t::LEDC_CHANNEL_3, 255 - val);
            // ledc_update_duty(ledc_mode_t::LEDC_LOW_SPEED_MODE, ledc_channel_t::LEDC_CHANNEL_3);
            break;
        }
        return true;
    };

    // делегирование настроек контрастности дисплея
    setting_args_int_range *_contrastSettingsArg; // = setting_args_int_range(0, "d_co", 50, 200, 190);
    ShareSetting _contrastSetting = ShareSetting(LOC_CONTRAST, _contrastSettingsArg, _onSettingChangeEvent);

    // делегирование настроек яркости подсветки дисплея
    setting_args_int_range *_brightnessSettingsArg; // = setting_args_int_range(1, "d_br", 0, 100, 80);
    ShareSetting _brightnessSetting = ShareSetting(LOC_BRIGHTNESS, _brightnessSettingsArg, _onSettingChangeEvent);

public:
    Nokia5110_U8g2()
    {
        _u8g2 = new U8G2_PCD8544_84X48_F_4W_SW_SPI(U8G2_R0, /* clock=*/GPIO_NUM_19, /* data=*/GPIO_NUM_18,
                                                   /* cs=*/GPIO_NUM_17, /* dc=*/GPIO_NUM_5, /* reset=*/GPIO_NUM_16);

        addSetting(&_contrastSetting)->addSetting(&_brightnessSetting);
    }

    ~Nokia5110_U8g2()
    {
        delete _u8g2;
        _u8g2 = nullptr;

        delete _interfaceEngine;
        _interfaceEngine = nullptr;

        delete _contrastSettingsArg;
        delete _brightnessSettingsArg;
        _contrastSettingsArg = nullptr;
        _brightnessSettingsArg = nullptr;
    }

    void init() override
    {
        Serial.println("Display width: " + String(_u8g2->getWidth()));

        _contrastSettingsArg = new setting_args_int_range(0, "d_co", 50, 200, 190);
        _brightnessSettingsArg = new setting_args_int_range(1, "d_br", 0, 100, 80);

        uint8_t val = (uint8_t)((float)255 * (_brightnessSettingsArg->currentVal / 100.0));
        //  Подсветка дисплея
        ledcSetup(3, 1000, 8);
        ledcAttachPin(DISPLAY_LED_PIN, 3);
        ledcWrite(3, 255 - val);

        _u8g2->begin();

        _u8g2->setContrast(_contrastSettingsArg->currentVal);

        _u8g2->enableUTF8Print();

        U8g2DisplayVirtual::init(); // Там инициализация джвижка прорисовки
    }

    /// @brief Получить тип подключения дисплея
    /// @return DIPLAY_TYPE_SPI
    display_type getType() override { return DISPLAY_TYPE_SPI; }

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_WIDE
    display_interface_type getUserInterfaceType() override { return DISPLAY_INTERFACE_TYPE_WIDE; }

    const uint8_t *getFontForSize(el_text_size size) override
    {
        const uint8_t *ret = nullptr;
        switch (size)
        {
        case EL_VOLTMETER_VALUE_LARGE:
            ret = u8g2_font_ncenB18_tr; // u8g2_font_osb41_tf
            break;
        case EL_TEXT_SIZE_SUPER_LARGE:
            ret = u8g2_font_10x20_t_cyrillic;
            break;
        case EL_TEXT_SIZE_LARGE:
            ret = u8g2_font_8x13_t_cyrillic;
            break;
        case EL_TEXT_SIZE_MIDDLE:
            ret = u8g2_font_6x12_t_cyrillic;
            break;
        case EL_TEXT_SIZE_SMALL:
            ret = u8g2_font_5x7_t_cyrillic;
            break;
        case EL_TEXT_SIZE_SUPER_SMALL:
            ret = u8g2_font_4x6_t_cyrillic;
            break;
        }

        return ret;
    }
};
