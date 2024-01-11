#pragma once

#include "ellements_structs.h"
#include "displays/display_structs.h"

class EllementVirtual {
    protected:
    bool _visible;
    display_area _area;
    private:

    public:
    EllementVirtual(){

    }

    virtual bool isGroup(){
        return false;
    }

    virtual void setVisibility(bool visible){
        _visible = visible;
    }

    virtual bool isVisible(){
        return _visible;
    }

    virtual void setArea(display_area area){
        _area = area;
    }

    virtual display_area getArea(){
        return _area;
    }

    virtual int getX(){
        return _area.leftUp.x;
    }

    virtual int getY(){
        return _area.leftUp.y;
    }

    virtual int getWidth(){
        return _area.getWidth();
    }

    virtual int getHeight(){
        return _area.getHeight();
    }

    virtual el_type getEllementType() = 0;

};