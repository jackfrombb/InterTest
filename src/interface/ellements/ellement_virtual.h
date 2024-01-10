#pragma once

#include "ellements_structs.h"
#include "displays/display_structs.h"

/// @brief Для относитильного позиционирования эллементов
typedef struct {
    EllementVirtual* leftTo;
    EllementVirtual* rightTo;
    EllementVirtual* topTo;
    EllementVirtual* bottomTo;
} relative_position;

class EllementVirtual {
    protected:
    bool _visible;
    display_area _area;
    el_text_size _elSize;
    private:

    public:
    EllementVirtual(){

    }

    EllementVirtual(el_text_size elSize){
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

    virtual void setTextSize(el_text_size size){
        _elSize = size;
    }

    virtual el_text_size getEllementSize(){
        return _elSize;
    }

};