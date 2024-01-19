#pragma once

#include "ellements_structs.h"
#include "displays/display_structs.h"

class ElementVirtual
{
protected:
    bool _visible = true;
    display_position _area{};
    ElementVirtual *_parent = nullptr;
    el_vertical_align _vertAlign = el_vertical_align::EL_ALGN_TOP;

private:
public:
    ElementVirtual() = default;

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
        _area.leftUp.x = x;
        return this;
    }

    virtual int getY()
    {
        return _area.leftUp.y;
    }

    virtual ElementVirtual *setY(int y)
    {
        _area.leftUp.y = y;
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

    virtual void onDraw()
    {
    }

    virtual bool onControl(control_event_type type, void *args)
    {
        return false;
    }

    virtual el_type getElementType() = 0;
};