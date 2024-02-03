#pragma once
//#include "display_virtual.h"
#include "interface/engines/interface_engine_u8g2.h"

class U8g2DisplayVirtual : public DisplayVirtual
{
protected:
    U8G2 *_u8g2;

public:
    U8g2DisplayVirtual(/* args */)
    {
        _interfaceEngine = new InterfaceEngine_U8g2(this);
    }
    ~U8g2DisplayVirtual() override
    {
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
};
