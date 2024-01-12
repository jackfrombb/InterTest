#pragma once
#include <Arduino.h>
#include "ellement_virtual.h"

typedef struct {
    void (*event) (void* args);
    void* args;
} ui_event;

class ElTextButton : public EllementVirtual
{
private:
    String _title;  // надпись на кнопке
    bool _pushed;   // кнопка нажата
    bool _selected; // кнопка выбрана
    ui_event _event; // событие по нажатию кнопки
    el_text_size _textSize;

public:
    ElTextButton()
    {
    }

    ElTextButton(String title, display_position area, el_text_size size) : EllementVirtual()
    {
        _title = title;
        setArea(area);
    }
    ~ElTextButton()
    {
    }

    void setEvent(ui_event event){
        _event = event;
    }

    ui_event getEvent(){
        return _event;
    }

    void setTitle(String title){
        _title = title;
    }

    String* getTitle(){
        return &_title;
    }

    void setPushed(bool pushed){
        _pushed = pushed;
        if(_pushed){
            _event.event(_event.args);
        }
    }

    bool isPushed(){
        return _pushed;
    }

    void setSelected(bool selected){
        _selected = selected;
    }

    bool isSelected(){
        return _selected;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_BUTTON;
    }
};