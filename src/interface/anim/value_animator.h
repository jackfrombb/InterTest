#pragma once
#include <Arduino.h>

//Заготовка не используется
template <typename T>
class ValueAnimator
{
private:
    T *_val;
    bool _trigger;
    bool _timerOn;
    ulong _lastTriggeredTime;ы
    int _triggerOffTime;

    T _from;
    T _to;

public:
    ValueAnimator()
    {
    }

    void show(bool show){
        _trigger = show;
    }

    void setValue(T *val, T from, T to, T steep, bool timerOn = false, int triggerOffTime = 3000)
    {
        _val = val;
        _trigger = trigger;
        _from = from;
        _to = to;

        if (timerOn)
        {
            _lastTriggeredTime = millis();
            _triggerOffTime = triggerOffTime;
        }
    }

    void onDraw()
    {
        if (*_trigger && _val < _to)
        {
            val += steep;
        }
        else if (!*_trigger && _val > from)
        {
            val -= steep;
        }

        if (_trigger && _lastTriggeredTime - millis() > _triggerOffTime)
        {
            show(false);
        }
    }
};