#pragma once

#include "ellements_structs.h"
#include "displays/display_structs.h"

class EllementVirtual
{
protected:
    bool _visible = true;
    display_position _area;
    EllementVirtual *_root = nullptr;
    EllementVirtual *_parent = nullptr;
    el_vertical_align _vertAlign;

private:
public:
    static el_text_px_area TEXT_SIZE_VALUES;

    EllementVirtual()
    {
        setArea({});
    }

    EllementVirtual(EllementVirtual *root, EllementVirtual *parent)
    {
        _root = root;
        _parent = parent;

        setArea({});
    }

    virtual bool isGroup()
    {
        return false;
    }

    virtual void setVisibility(bool visible)
    {
        _visible = visible;
    }

    virtual bool isVisible()
    {
        return _visible;
    }

    virtual EllementVirtual *setArea(display_position area)
    {
        _area = area;
        return this;
    }

    virtual display_position getArea()
    {
        return _area;
    }

    virtual display_position* getAreaPtr(){
        return &_area;
    }

    void setRoot(EllementVirtual *root)
    {
        _root = root;
    }

    EllementVirtual *getRoot()
    {
        return _root;
    }

    EllementVirtual *getParent()
    {
        return _parent;
    }

    void setParent(EllementVirtual *parent)
    {
        _parent = parent;
    }

    virtual int getX()
    {
        return _area.leftUp.x;
    }

    virtual EllementVirtual *setX(int x)
    {
        _area.leftUp.x = x;
        return this;
    }

    virtual int getY()
    {
        return _area.leftUp.y;
    }

    virtual EllementVirtual *setY(int y)
    {
        _area.leftUp.y = y;
        return this;
    }

    virtual int getWidth()
    {
        return _area.getWidth();
    }

    virtual EllementVirtual *setWidth(uint32_t width)
    {
        _area.setWidth(width);
        return this;
    }

    virtual int getHeight()
    {
        return _area.getHeight();
    }

    virtual EllementVirtual *setHeight(uint32_t height)
    {
        _area.setHeight(height);
        return this;
    }

    virtual EllementVirtual *setVertialAlignment(el_vertical_align vAlign)
    {
        _vertAlign = vAlign;
        return this;
    }

    virtual el_vertical_align getVerticalAlignment()
    {
        return _vertAlign;
    }

    virtual el_type getEllementType() = 0;
};

el_text_px_area EllementVirtual::TEXT_SIZE_VALUES;