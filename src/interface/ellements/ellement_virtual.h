#pragma once

#include "displays/display_structs.h"
#define ELLEMENT_POSITION_CENTER -1

typedef enum{
    EL_SIZE_SUPER_LARGE, EL_SIZE_LARGE, EL_SIZE_MIDDLE, EL_SIZE_SMALL, EL_SIZE_SUPER_SMALL
} el_size;

typedef enum {
    EL_TYPE_UNKNOWN, EL_TYPE_BUTTON, EL_TYPE_TEXT, 
    EL_TYPE_IMAGE, EL_TYPE_WAVEFORM, EL_TYPE_LINE, EL_TYPE_POINT,
    EL_TYPE_PROGRESS_BAR,
} el_type;

class EllementVirtual {
    protected:
    bool _visible;
    display_area _area;
    el_size _elSize;
    private:
    public:
    EllementVirtual(){

    }

    EllementVirtual(el_size elSize){
        _elSize = elSize;
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

    virtual void setEllementsSize(el_size size){
        _elSize = size;
    }

    virtual el_size getEllementSize(){
        return _elSize;
    }

};