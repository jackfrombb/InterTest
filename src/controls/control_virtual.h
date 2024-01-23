#pragma once
#include <Arduino.h>

typedef enum
{
    // Основные кнопки. Получается три физические кнопки или энкодер. Press back - предполагается как двойной клик на OK
    PRESS_OK,
    PRESS_BACK,
    PRESS_LEFT,
    PRESS_RIGHT,
    LONG_PRESS_LEFT,
    LONG_PRESS_RIGHT,
    LONG_PRESS_OK,
    // Реализация по возможности, ставить только быстрый доступ. С этими 5 кнопок, расчет на smd джойстик.
    PRESS_UP,
    PRESS_DOWN,
} control_event_type;

class ControlVirtual
{
protected:
    void *_args = nullptr;
    // Обработчик события. Возвращает true если событие обработано и false, что бы передать обработку на уровень вверх
    // Аргументы нужны для реализации в статическом методе класса
    bool (*_handler)(control_event_type eventType, void *args);

private:
public:
    ControlVirtual()
    {
    }
    ~ControlVirtual()
    {
        removeControlHandler();
    }

    virtual void init()
    {
    }

    virtual void loop()
    {
    }

    void attachControlHandler(bool (*handler)(control_event_type eventType, void *args), void *args)
    {
        _handler = handler;
        _args = args;
    }

    void removeControlHandler()
    {
        _handler = nullptr;
        _args = nullptr;
    }
};