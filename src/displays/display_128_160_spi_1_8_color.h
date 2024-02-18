/**
 * ПОДДЕРЖКА ДИСПЛЕЯ ОТМЕНЕНА до нахождения или написания достаточно быстрого способа вывода информации на экран
 *
 * @file display_128_160_spi_1_8_color.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Класс для подключения и инициализации дисплея на контроллере ST7735 TFT 128x160 (1.8, red plate with sd)
 * @version 0.1
 * @date 2024-02-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <Arduino.h>

#define GFXFF 1
#define FF18 &FreeSans12pt7b

#define DISP_DC GPIO_NUM_33 // A0 on board
#define DISP_CS GPIO_NUM_16
#define DISP_SCK GPIO_NUM_37
#define DISP_MOSI GPIO_NUM_35
#define DISP_RST GPIO_NUM_21
#define DISP_LED GPIO_NUM_38 // Подсветка дисплея. Можно управлять яркостью управляя скважностью шим

#include <TFT_eSPI.h>

#include "interface/engines/interface_engine_adafruit_gfx.h"
#include <SPI.h>

// TODO: отключить библиотеки ниже после добавления в основной проект
#include "module_virtual.h"
#include "display_virtual.h"
#include "displays/display_structs.h"
#include "interface/ellements/ellements_structs.h"

class Display128x160_1_8_Spi_Color : public DisplayVirtual
{
private:
    TFT_eSPI *tft;

public:
    Display128x160_1_8_Spi_Color()
    {
        tft = new TFT_eSPI();
    }

    void init() override
    {
        // Запитываем подсветку от пина (можно управлять яркостью)
        pinMode(DISP_LED, OUTPUT);
        digitalWrite(DISP_LED, HIGH);

        // Инициализация дисплея
        tft->init();

        tft->setRotation(3);        // Устанавливаем ориентацию дисплея
        tft->fillScreen(TFT_BLACK); // Заполняем экран черным цветом

        _interfaceEngine = new InterfaceEngine_ArduinoGfx(this);
    }

    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    display_type getType() override
    {
        return display_type::DISPLAY_TYPE_SPI;
    }

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    display_resolution getResolution() override
    {
        return display_resolution{
            .width = (uint16_t)tft->width(),
            .height = (uint16_t)tft->height(),
        };
    }

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_UNKNOWN,DISPLAY_INTERFACE_TYPE_WIDE,DISPLAY_INTERFACE_TYPE_SLIM,
    display_interface_type getUserInterfaceType() override
    {
        return display_interface_type::DISPLAY_INTERFACE_TYPE_WIDE;
    }

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_UNKNOWN, DISPLAY_LIB_U8G2,
    display_library getDisplayLibraryType() override
    {
        return display_library::DISPLAY_LIB_ARDUINO_GFX;
    }

    /// @brief Получить библиотеку дисплея
    /// @return Основной класс библиотеки, в зависимости от типа
    void *getLibrary() override
    {
        return tft;
    }

    const uint8_t *getFontForSize(el_text_size size) override
    {
        // static uint8_t ret = 0;

        //tft->setFreeFont(FF18);

        // switch (size)
        // {
        // case EL_VOLTMETER_VALUE_LARGE:
        //     tft->setTextSize(8);
        //     break;
        // case EL_TEXT_SIZE_SUPER_LARGE:
        //     tft->setTextSize(7);
        //     break;
        // case EL_TEXT_SIZE_LARGE:
        //     tft->setTextSize(6);
        //     break;
        // case EL_TEXT_SIZE_MIDDLE:
        //     tft->setTextSize(4);
        //     break;
        // case EL_TEXT_SIZE_SMALL:
        //     tft->setTextSize(2);
        //     break;
        // case EL_TEXT_SIZE_SUPER_SMALL:
        //     tft->setTextSize(1);
        //     break;
        // }

        return nullptr;
    }
};