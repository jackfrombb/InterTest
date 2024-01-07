#pragma once

#include "displays/display_structs.h"

typedef enum{
    EL_SIZE_FULLSCREEN, EL_SIZE_SUPER_LARGE, EL_SIZE_LARGE, EL_SIZE_MIDDLE, EL_SIZE_SMALL, EL_SIZE_SUPER_SMALL
} el_size;

typedef enum {
    EL_TYPE_UNKNOWN, EL_TYPE_BUTTON, EL_TYPE_TEXT, EL_TYPE_IMAGE, EL_TYPE_WAVEFORM, EL_TYPE_LINE, EL_TYPE_POINT
} el_type;

class EllementVirtual {
    protected:
    bool _visible;
    display_area _area;
    point_t _areaOuterPosition;
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

    virtual point_t getAareaOuterPosition(){
        return _areaOuterPosition;
    }

    virtual void setAreaOuterPosition(point_t areaPos){
        _areaOuterPosition = areaPos;
    }

    virtual el_type getEllementType(){
        return EL_TYPE_UNKNOWN;
    }

    virtual void setEllementsSize(el_size size){
        _elSize = size;
    }

    virtual el_size getEllementSize(){
        return _elSize;
    }

};