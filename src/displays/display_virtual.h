#pragma once
#include <Arduino.h>
#include "module_virtual.h"

/// @brief Тип подключения дисплея
typedef enum
{
    DISPLAY_TYPE_UNKNOWN,
    DISPLAY_TYPE_SPI,
    DISPLAY_TYPE_I2C
} display_type;

/// @brief Тип библиотеки для вывода информации на  дисплей
typedef enum
{
    DISPLAY_LIB_UNKNOWN,
    DISPLAY_LIB_U8G2,
} display_library;

/// @brief Тип интерфейса
typedef enum
{
    DISPLAY_INTERFACE_TYPE_UNKNOWN,
    DISPLAY_INTERFACE_TYPE_WIDE,
    DISPLAY_INTERFACE_TYPE_SLIM,
} display_interface_type;

/// @brief Разрешение дисплея
typedef struct
{
    uint16_t width;
    uint16_t height;
} display_resolution;

/// @brief Виртуальный класс, основа для подключения разных дисплеев
class DisplayVirtual : public ModuleVirtual
{
protected:
private:
public:
    DisplayVirtual(/* args */)
    {
    }
    ~DisplayVirtual()
    {
    }

    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    virtual display_type getType() = 0;

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    virtual display_resolution getResoluton() = 0;

    /// @brief Получить отличительную характеристику дисплея (для определения типа интерфейса)
    /// @return DISPLAY_INTERFACE_TYPE_UNKNOWN,DISPLAY_INTERFACE_TYPE_WIDE,DISPLAY_INTERFACE_TYPE_SLIM,
    virtual display_interface_type getUserInterfaceType() = 0;

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_UNKNOWN, DISPLAY_LIB_U8G2,
    virtual display_library getDisplayLibraryType() = 0;

    /// @brief Получить библиотеку дисплея
    /// @return Основной класс библиотеки, в зависимости от типа
    virtual void* getLibrarry() = 0;
};
