#pragma once
#include <Arduino.h>

typedef enum { 
    //Основные кнопки. Получается три физические кнопки или энкодер
    PRESS_OK, PRESS_LEFT, PRESS_RIGHT, LONG_PRESS_LEFT, LONG_PRESS_RIGHT, LONG_PRESS_OK, 
    //Реализация по возможности, ставить только быстрый доступ. С этими 5 кнопок, расчет на smd джойстик.
    PRESS_UP, PRESS_DOWN, 
} control_event_type;

typedef struct
{
    void (*event)(control_event_type eventType, void *args);
    void *args;
} control_event;



class ControlVirtual
{
protected:
    control_event* _controlEvent;

private:
public:
    ControlVirtual()
    {
        _controlEvent = nullptr;
    }
    ~ControlVirtual()
    {
        removeControlEvent();
    }

    virtual void init(){}

    virtual void loop(){}

    void setControlEvent(control_event* event){
        _controlEvent = event;
    }

    void removeControlEvent(){
        delete _controlEvent;
        _controlEvent = nullptr;
    }

};