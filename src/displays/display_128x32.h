#pragma once
#include <Arduino.h>
#include "display_helper.h"
#include "display_virtual.h"
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCONTRAST 0x81

// дисплей 0.96 OLED I2C
#define DSP_SDA_ 26
#define DSP_SCK_ 27

#define DISPU8X8_

// pinMode( DSP_SCK_, OUTPUT );
// pinMode( DSP_SDA_, OUTPUT );
// U8G2_SH1106_128X32_VISIONOX_F_HW_I2C loc_dsp(U8G2_R0, U8X8_PIN_NONE, DSP_SCK_, DSP_SDA_);
// U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, DSP_SCK_, DSP_SDA_, U8X8_PIN_NONE);

// void display_init()
// {
//     // u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 180);
//     // half u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 90);
//     // u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 45);
//     u8g2.setContrast(255);
//     u8g2.setPowerSave(0);
//     u8g2.begin();
//     u8g2.enableUTF8Print();
// }

class Display128x64_U8g2 : public DisplayVirtual
{
private:
    U8G2 *_u8g2;

public:
    Display128x64_U8g2()
    {

        // pinMode( DSP_SCK_, OUTPUT );
        // pinMode( DSP_SDA_, OUTPUT );
        // U8G2_SH1106_128X32_VISIONOX_F_HW_I2C loc_dsp(U8G2_R0, U8X8_PIN_NONE, DSP_SCK_, DSP_SDA_);
        // U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, DSP_SCK_, DSP_SDA_, U8X8_PIN_NONE);
        _u8g2 = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, DSP_SCK_, DSP_SDA_, U8X8_PIN_NONE);
    }

    ~Display128x64_U8g2()
    {
        delete _u8g2;
    }

    void init()
    {
        Serial.println("Display width: " + String(_u8g2->getWidth()));
        // u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 180);
        // half u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 90);
        // u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 45);
        _u8g2->setContrast(255);
        _u8g2->setPowerSave(0);
        _u8g2->begin();
        _u8g2->enableUTF8Print();
    }

    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    virtual display_type getType() { return DISPLAY_TYPE_I2C; }

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    virtual display_resolution getResoluton() { return display_resolution{
        .width = _u8g2->getWidth(),
        .height = _u8g2->getHeight(),
    }; }

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_UNKNOWN,DISPLAY_INTERFACE_TYPE_WIDE,DISPLAY_INTERFACE_TYPE_SLIM,
    virtual display_interface_type getUserInterfaceType() { return DISPLAY_INTERFACE_TYPE_WIDE; }

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_UNKNOWN, DISPLAY_LIB_U8G2,
    virtual display_library getDisplayLibraryType() { return DISPLAY_LIB_U8G2; }
};