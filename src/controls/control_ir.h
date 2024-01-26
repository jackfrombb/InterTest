#pragma once
#include "controls/control_virtual.h"
#include <NecDecoder.h>

#define CONTROL_IR_S GPIO_NUM_14

class ControlIr : public ControlVirtual
{
protected:
    static NecDecoder *ir;

    IRAM_ATTR static void _irIsr()
    {
        ir->tick();
    }

public:
    ControlIr(/* args */)
    {
    }
    ~ControlIr()
    {
        delete ir;
        ir = nullptr;
    }

    void init() override
    {
        attachInterrupt(CONTROL_IR_S, _irIsr, FALLING);
    }

    /*
        0xFF18E7 - вверх
        0xFF5AA5 - вправо
        0xFF10EF - влево
        0xFF4AB5 - вниз
        0xFF38C7 - ОК
        0xFF6897 - звездочка
        0xFFB04F - решётка

    */
    void loop() override
    {
        if (ir->available())
        {
            // выводим весь пакет (32 бита)
            Serial.print("0x");
            Serial.println(ir->readPacket(), HEX);

            // можно вывести только команду (8 бит)
            // Serial.println(ir.readCommand(), HEX);

            switch (ir->readPacket())
            {
            case 0xFF10EF:
                _handler(control_event_type::PRESS_LEFT, _args);
                break;
                
            case 0xFF5AA5:
                _handler(control_event_type::PRESS_RIGHT, _args);
                break;

            case 0xFF38C7:
                _handler(control_event_type::PRESS_OK, _args);
                break;

            case 0xFF6897:
                _handler(control_event_type::LONG_PRESS_OK, _args);
                break;

            case 0xFFB04F:
                _handler(control_event_type::PRESS_BACK, _args);
                break;

                
            }
        }
    }
};

NecDecoder *ControlIr::ir = new NecDecoder();
