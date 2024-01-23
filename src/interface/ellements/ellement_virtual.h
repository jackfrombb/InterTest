#pragma once

#include <cmath>
#include "ellements_structs.h"
#include "displays/display_structs.h"
#include "controls/control_virtual.h"
#include "logi.h"

using namespace std;

class ElementVirtual
{
protected:
    bool _visible = true;
    display_position _area{};
    ElementVirtual *_parent = nullptr;
    el_vertical_align _vertAlign = el_vertical_align::EL_ALGN_TOP;
    bool _isAnimationInProcess = false;

    display_position _flyToArea{};
    float _stepInFrameX = 1.0;
    float _stepInFrameY = 1.0;
    void *_animArgs = nullptr;
    function<void(ElementVirtual *, void *)> _onAnimationEnd = nullptr;

private:
public:
    ElementVirtual() = default;
    virtual ~ElementVirtual() = default;

    explicit ElementVirtual(ElementVirtual *parent)
    {
        _parent = parent;
    }

    virtual bool isGroup()
    {
        return false;
    }

    virtual ElementVirtual *setVisibility(bool visible)
    {
        _visible = visible;
        return this;
    }

    virtual bool isVisible()
    {
        return _visible;
    }

    virtual ElementVirtual *setArea(display_position area)
    {
        _area = area;
        return this;
    }

    virtual display_position getArea()
    {
        return _area;
    }

    virtual display_position *getAreaPtr()
    {
        return &_area;
    }

    ElementVirtual *getParent()
    {
        return _parent;
    }

    void setParent(ElementVirtual *parent)
    {
        _parent = parent;
    }

    virtual int getX()
    {
        return _area.leftUp.x;
    }

    virtual ElementVirtual *setX(int x)
    {
        _area.setX(x);
        return this;
    }

    virtual int getY()
    {
        return _area.leftUp.y;
    }

    virtual ElementVirtual *setY(int y)
    {
        _area.setY(y);
        return this;
    }

    virtual int getWidth()
    {
        return _area.getWidth();
    }

    virtual ElementVirtual *setWidth(uint32_t width)
    {
        _area.setWidth(width);
        return this;
    }

    virtual int getHeight()
    {
        return _area.getHeight();
    }

    virtual ElementVirtual *setHeight(uint32_t height)
    {
        _area.setHeight(height);
        return this;
    }

    virtual ElementVirtual *setVerticalAlignment(el_vertical_align vAlign)
    {
        _vertAlign = vAlign;
        return this;
    }

    virtual el_vertical_align getVerticalAlignment()
    {
        return _vertAlign;
    }

    virtual bool isAnimationInProcess()
    {
        return _isAnimationInProcess;
    }

    virtual void flyTo(int x, int y, uint16_t frameTime,
                       function<void(ElementVirtual *, void *)> onAnimationEnd = nullptr, void *animArgs = nullptr)
    {
        _flyToArea = {
            .leftUp = {.x = x, .y = y},
        };
        _flyToArea.setWidth(getWidth());
        _flyToArea.setHeight(getHeight());

        _stepInFrameX = (getX() - x) / frameTime;
        _stepInFrameY = (getY() - y) / frameTime;

        _onAnimationEnd = onAnimationEnd;
        _animArgs = animArgs;

        _isAnimationInProcess = true;
    }

    virtual void nextAnimStep()
    {
        if (_isAnimationInProcess) // Если анимация не окончена
        {
            // Узнаем оставшуюся разницу
            int diffX = _flyToArea.getX() - getX();
            int diffY = _flyToArea.getY() - getY();

            // Если не закончили с х то либо делаем шаг, либо добираем (смотря что меньше)
            if (diffX != 0)
            {
                setX((abs(_stepInFrameX) < abs(diffX) ? (getX() - _stepInFrameX) : _flyToArea.getX()));
            }

            // Так же и с y
            if (diffY != 0)
            {
                setY((abs(_stepInFrameY) < abs(diffY) ? (getY() - _stepInFrameY) : _flyToArea.getY()));
            }

            // Если со всем закончили то выключаем анимацию, что бы не проверять каждый цикл переменные
            if (getX() == _flyToArea.getX() && getY() == _flyToArea.getY())
            {

                _isAnimationInProcess = false;

                if (_onAnimationEnd != nullptr)
                    _onAnimationEnd(this, _animArgs);

                _onAnimationEnd = nullptr;
            }

            // String msgS = (_isAnimationInProcess ? "Animation in process " : "Animation end ");
            // logi::p("ElementVirtual", msgS +
            //                               " diffX: " + String(diffX) +
            //                               // " diffY: " + String(diffY) +
            //                               " X: " + String(getX()) +
            //                               " To X: " + String(_flyToArea.getX()));
            // " Y: " + String(getY()));
        }
    }

    virtual void onDraw()
    {
    }

    virtual bool onControl(control_event_type type, void *args)
    {
        return false;
    }

    virtual el_type getElementType() = 0;
};