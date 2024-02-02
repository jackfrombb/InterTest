#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "display_helper.h"
#include "display_virtual.h"
#include "logi.h"

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// дисплей 0.96 OLED I2C
#define DSP_SDA GPIO_NUM_33
#define DSP_SCK GPIO_NUM_21
#define DISP_VCC GPIO_NUM_16

class Display128x64_U8g2 : public DisplayVirtual
{
private:
    U8G2 *_u8g2;

public:
    Display128x64_U8g2()
    {
        _u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE, DSP_SCK, DSP_SDA);
    }

    ~Display128x64_U8g2()
    {
        delete _u8g2;
        _u8g2 = nullptr;
    }

    void init()
    {
        //logi::p("Display 128x64", "Display width: " + String(_u8g2->getWidth()));

        pinMode(DISP_VCC, OUTPUT);
        digitalWrite(DISP_VCC, 1);

        _u8g2->begin();
        _u8g2->enableUTF8Print();
        _u8g2->setContrast(100);
        logi::p("Display 128x64", "Display init ok");
    }

    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    display_type getType() override { return DISPLAY_TYPE_I2C; }

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    display_resolution getResolution() override { return display_resolution{
        .width = _u8g2->getWidth(),
        .height = _u8g2->getHeight(),
    }; }

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_UNKNOWN,DISPLAY_INTERFACE_TYPE_WIDE,DISPLAY_INTERFACE_TYPE_SLIM,
    display_interface_type getUserInterfaceType() override { return DISPLAY_INTERFACE_TYPE_WIDE; }

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_UNKNOWN, DISPLAY_LIB_U8G2,
    display_library getDisplayLibraryType() override { return DISPLAY_LIB_U8G2; }

    void *getLibrary() override
    {
        return _u8g2;
    }
};