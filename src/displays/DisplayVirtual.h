//
// Created by Admin on 08.01.2024.
//

#include <Arduino.h>
#include "interface/engines/interface_engine.h"
#include "displays/display_structs.h"

class DisplayVirtual {

public:
    DisplayVirtual();
    ~DisplayVirtual();
    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    virtual display_type getType();

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    virtual display_resolution getResolution() = 0;

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_UNKNOWN,DISPLAY_INTERFACE_TYPE_WIDE,DISPLAY_INTERFACE_TYPE_SLIM,
    virtual display_interface_type getUserInterfaceType() = 0;

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_UNKNOWN, DISPLAY_LIB_U8G2,
    virtual display_library getDisplayLibraryType() = 0;

    /// @brief Получить библиотеку дисплея
    /// @return Основной класс библиотеки, в зависимости от типа
    virtual void* getLibrary() = 0;
    virtual InterfaceEngineVirtual* getInterfaceEngine() = 0;

    virtual void init() = 0;
};
