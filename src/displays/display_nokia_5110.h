#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "display_helper.h"
#include "display_virtual.h"
#include "interface/engines/interface_engine_u8g2.h"

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
class Nokia5110_U8g2 : public DisplayVirtual
{
private:
    U8G2 *_u8g2;

public:
    Nokia5110_U8g2()
    {
        _u8g2 = new U8G2_PCD8544_84X48_F_4W_SW_SPI(U8G2_R0, /* clock=*/GPIO_NUM_19, /* data=*/GPIO_NUM_18,
                                                   /* cs=*/GPIO_NUM_17, /* dc=*/GPIO_NUM_5, /* reset=*/GPIO_NUM_16);
    }

    ~Nokia5110_U8g2()
    {
        delete _u8g2;
    }

    void init() override
    {
        Serial.println("Display width: " + String(_u8g2->getWidth()));

        //  Подсветка дисплея
        ledcSetup(3, 100, 8);
        ledcAttachPin(DISPLAY_LED_PIN, 3);
        ledcWrite(3, 150);

        _u8g2->begin();
        _u8g2->enableUTF8Print();
    }

    /// @brief Получить тип подключения дисплея
    /// @return DIPLAY_TYPE_SPI
    display_type getType() override { return DISPLAY_TYPE_SPI; }

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    display_resolution getResolution() override { return display_resolution{
        .width = _u8g2->getWidth(),
        .height = _u8g2->getHeight(),
    }; }

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_WIDE
    display_interface_type getUserInterfaceType() override { return DISPLAY_INTERFACE_TYPE_WIDE; }

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_U8G2
    display_library getDisplayLibraryType() override { return DISPLAY_LIB_U8G2; }

    /// @brief Получить библиотеку дисплея
    /// @return Основной класс библиотеки, в зависимости от типа
    void *getLibrary() override { return _u8g2; }
};
