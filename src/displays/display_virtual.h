#pragma once
#include <Arduino.h>

class InterfaceEngineVirtual; // Определяем прототип. Костыль, что бы виртуальный дисплей знал про движок отрисовки

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
    DISPLAY_LIB_ARDUINO_GFX,
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
class DisplayVirtual : public ModuleVirtual, public iHaveShareSettings
{
protected:
    InterfaceEngineVirtual *_interfaceEngine;

    // Варианты направления дисплея
    vector<int> _dispDirection = {0, 1, 2, 3};
    // Аргументы настройки поворота дисплея
    setting_args_int_steep *_rotationSettingsValue; // = setting_args_int_steep(0, "dsp_rot", &_dispDirection, 0);
    // Настройка поворота дисплея для передачи в раздел "Настройки"
    ShareSetting *_rotationSetting;

private:
public:
    DisplayVirtual(/* args */) : iHaveShareSettings(LOC_DISPLAY)
    {
    }
    virtual ~DisplayVirtual()
    {
    }

    void init() override
    {
        _rotationSettingsValue = new setting_args_int_steep(0, "dsp_rot", &_dispDirection, 0);
        _rotationSetting = new ShareSetting(LOC_ROTATION, _rotationSettingsValue,
                                            [this](settings_args_virtual *args)
                                            {
                                                setting_args_int_steep *settingArgs = (setting_args_int_steep *)args;
                                                setDisplayDirection(settingArgs->getSteepValue());
                                               // tft->setRotation(settingArgs->getSteepValue()); // Устанавливаем ориентацию дисплея

                                                return true;
                                            });

        addSetting(_rotationSetting);
        _rotationSetting->onChange(); // Применяем настройки поворота дисплея
    }

    /// @brief Получить тип подключения дисплея
    /// @return DISPLAY_TYPE_UNKNOWN, DIPLAY_TYPE_SPI, DISPLAY_TYPE_I2C
    virtual display_type getType() = 0;

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
    virtual void *getLibrary() = 0;

    virtual uint16_t getWidth()
    {
        return getResolution().width;
    }

    virtual uint16_t getHeight()
    {
        return getResolution().height;
    }

    virtual void setDisplayDirection(uint8_t direction) = 0;

    virtual const uint8_t *getFontForSize(el_text_size size) = 0;

    virtual uint8_t getMaxTextWidth(el_text_size textSize) = 0;

    virtual uint8_t getMaxTextHeight(el_text_size textSize) = 0;

    virtual InterfaceEngineVirtual *getInterfaceEngine() { return _interfaceEngine; }
};
