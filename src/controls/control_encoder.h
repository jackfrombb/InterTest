#pragma once
#include "configuration.h"
#include "control_virtual.h"
#include "esp32-hal-gpio.h"
#include <EncButton.h>
#include "logi.h"
#include "hard_timer.h"

//  Энкодеры отличаются по способу коммуникации, а потому требуют дополнительного типа
#define ENCODER_TYPE2 // Это энкодеры с низким профилем.

#ifdef S2MINI
// Энкодер
// #define ENC_VCC GPIO_NUM_38
#define ENC_CLCK GPIO_NUM_1
#define ENC_DT GPIO_NUM_3
#define ENC_SW GPIO_NUM_14 // Кнопка
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
    HardTimer *_encoderTimer;

    void _encEvent()
    {
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

        switch (_enc->action())
        {
        case EB_HOLD:
            _handler(LONG_PRESS_OK, _args);
            break;

            // case EB_CLICK:
            //     _handler(PRESS_OK, _args);
            //     break;

        case EB_CLICKS:
            if (_enc->getClicks() == 1)
            {
                _handler(PRESS_OK, _args);
            }
            else if (_enc->getClicks() == 2)
            {
                bool isHandled = _handler(control_event_type::PRESS_BACK, _args);
            }
            break;

        case EB_TURN:
            if (_enc->encHolding())
            {
                _handler(_enc->dir() > 0 ? LONG_PRESS_RIGHT : LONG_PRESS_LEFT, _args);
            }
            else
            {
                _handler(_enc->dir() > 0 ? PRESS_RIGHT : PRESS_LEFT, _args);
            }
            break;
        }
    }

public:
    ControlEncoder()
    {
        _enc = new EncButton(ENC_DT, ENC_CLCK, ENC_SW);

#ifdef WROOM32
        _enc->setEncType(EB_STEP4_LOW);
#elif defined(S2MINI)
        _enc->setEncType(EB_STEP2);
#endif

        _encoderTimer = new HardTimer(encTick, TIMER_GROUP_1, TIMER_0, 600, 80);
    }

    ~ControlEncoder()
    {
        delete _enc;
        delete _encoderTimer;
    }

    void init() override
    {
#ifdef ENC_VCC // Если необходимо запитывать, иначе пин питания подключается к питанию контроллера
        pinMode(ENC_VCC, OUTPUT);
        digitalWrite(ENC_VCC, 1);
#endif

        _encoderTimer->setArgs(this);
        _encoderTimer->init();
    }

    void loop() override
    {
        if (_enc->tick())
            _encEvent();
    }

    /// @brief Прерывание для обработки пропущенных считываний энкодера
    /// @param args = NULL
    /// @return nothing
    static bool IRAM_ATTR encTick(void *args)
    {
        if (args != nullptr)
        {
            auto *control = (ControlEncoder *)args;
            control->_enc->tickISR();
        }
        return false;
    }
};