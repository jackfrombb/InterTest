#pragma once
#include "configuration.h"
#include "control_virtual.h"
#include "esp32-hal-gpio.h"
#include "EncButton.h"

#ifdef S2MINI
// Энкодер
#define ENC_VCC GPIO_NUM_38
#define ENC_CLCK GPIO_NUM_37
#define ENC_DT GPIO_NUM_39
#define ENC_SW GPIO_NUM_40 // Кнопка
#endif

#ifdef WROOM32
#define ENC_VCC GPIO_NUM_12
#define ENC_CLCK GPIO_NUM_26
#define ENC_DT GPIO_NUM_27
#define ENC_SW GPIO_NUM_14 // Кнопка
#endif

class ControlEncoder : public ControlVirtual
{
private:
    EncButton *_enc;
    bool IRAM_ATTR missTick(void *args);
    HardTimer *_encoderTimer;

    void static _encEvent(VirtButton *but, void *args)
    {
        EncButton *enc = (EncButton *)but;

        ControlEncoder *owner = (ControlEncoder *)args;
        // EB_PRESS - нажатие на кнопку
        // EB_HOLD - кнопка удержана
        // EB_STEP - импульсное удержание
        // EB_RELEASE - кнопка отпущена
        // EB_CLICK - одиночный клик
        // EB_CLICKS - сигнал о нескольких кликах
        // EB_TURN - поворот энкодера
        // EB_REL_HOLD - кнопка отпущена после удержания
        // EB_REL_HOLD_C - кнопка отпущена после удержания с предв. кликами
        // EB_REL_STEP - кнопка отпущена после степа
        // EB_REL_STEP_C - кнопка отпущена после степа с предв. кликами

        if (owner->_handler != nullptr)
            switch (enc->action())
            {
            case EB_HOLD:
                owner->_handler(LONG_PRESS_OK, owner->_args);
                break;

            case EB_CLICK:
                owner->_handler(PRESS_OK, owner->_args);
                break;

            case EB_TURN:
                if (enc->encHolding())
                {
                    owner->_handler(enc->dir() > 0 ? LONG_PRESS_RIGHT : LONG_PRESS_LEFT, owner->_args);
                }
                else
                {
                    owner->_handler(enc->dir() > 0 ? PRESS_RIGHT : PRESS_LEFT, owner->_args);
                }
                break;
            }
    }

public:
    ControlEncoder()
    {
        _enc = new EncButton(ENC_DT, ENC_CLCK, ENC_SW);
        _encoderTimer = new HardTimer(encTick, TIMER_GROUP_1, TIMER_0, 600, 80);
    }

    ~ControlEncoder()
    {
        delete _enc;
        delete _encoderTimer;
    }

    virtual void init()
    {
        pinMode(ENC_VCC, OUTPUT);
        digitalWrite(ENC_VCC, 1);

        _enc->attach(_encEvent, this);

        _encoderTimer->setArgs(this);
        _encoderTimer->init();
    }

    virtual void loop()
    {
        _enc->tick();
    }

    /// @brief Прерывание для обработки пропущенных считываний энкодера
    /// @param args = NULL
    /// @return nothing
    static bool IRAM_ATTR encTick(void *args)
    {
        if (args != nullptr)
        {
            ControlEncoder *control = (ControlEncoder *)args;
            control->_enc->tickISR();
        }
        return false;
    }
};