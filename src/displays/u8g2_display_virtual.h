#pragma once
// #include "display_virtual.h"
#include "interface/engines/interface_engine_u8g2.h" // Реализация абстракции движка отрисовки

class U8g2DisplayVirtual : public DisplayVirtual
{
protected:
    U8G2 *_u8g2;

    // Варианты направления дисплея
    vector<int> _dispDirection = {0, 1, 2, 3};
    // Аргументы настройки поворота дисплея
    setting_args_int_steep *_rotationSettingsValue; // = setting_args_int_steep(0, "dsp_rot", &_dispDirection, 0);
    // Настройка поворота дисплея для передачи в раздел "Настройки"
    ShareSetting *_rotationSetting;

public:
    U8g2DisplayVirtual()
    {
    }
    ~U8g2DisplayVirtual() override
    {
        delete _rotationSettingsValue;
        _rotationSettingsValue = nullptr;
        delete _interfaceEngine;
        _interfaceEngine = nullptr;
    }

    void init() override
    {
        _interfaceEngine = new InterfaceEngine_U8g2(this);
        _rotationSettingsValue = new setting_args_int_steep(0, "dsp_rot", &_dispDirection, 0);
        _rotationSetting = new ShareSetting(LOC_ROTATION, _rotationSettingsValue,
                                            [this](settings_args_virtual *args)
                                            {
                                                setting_args_int_steep *settingArgs = (setting_args_int_steep *)args;
                                                Serial.println("Disp rotation set to : " + String(settingArgs->getSteepValue()));
                                                switch (settingArgs->getSteepValue())
                                                {
                                                case 0:
                                                    _u8g2->setDisplayRotation(U8G2_R0);
                                                    break;
                                                case 1:
                                                    _u8g2->setDisplayRotation(U8G2_R1);
                                                    break;
                                                case 2:
                                                    _u8g2->setDisplayRotation(U8G2_R2);
                                                    break;
                                                case 3:
                                                    _u8g2->setDisplayRotation(U8G2_R3);
                                                    break;
                                                }
                                                return true;
                                            });

        addSetting(_rotationSetting);
        _rotationSetting->onChange(); // Применяем настройки поворота дисплея
    }

    /// @brief Получить разрешение дисплея
    /// @return Структура с width и height
    display_resolution getResolution() override
    {
        return display_resolution{
            .width = _u8g2->getWidth(),
            .height = _u8g2->getHeight(),
        };
    }

    /// @brief Получить тип библиотеки использованный для подключения дисплея
    /// @return DISPLAY_LIB_U8G2
    display_library getDisplayLibraryType() override { return DISPLAY_LIB_U8G2; }

    /// @brief Получить библиотеку дисплея
    /// @return Основной класс библиотеки, в зависимости от типа
    void *getLibrary() override { return _u8g2; }

    uint8_t getMaxTextWidth(el_text_size textSize) override
    {
        return _interfaceEngine->getMaxTextWidth(textSize);
    }

    uint8_t getMaxTextHeight(el_text_size textSize) override
    {
        return _interfaceEngine->getMaxTextHeight(textSize);
    }
};
