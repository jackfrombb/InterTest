#pragma once

#include <cmath>
#include "element_style_tags.h" // Теги меняющие стиль отображения элемента
// #include "ellements_structs.h"
// #include "displays/display_structs.h"
//  #include "controls/control_virtual.h"
#include "logi.h"

using namespace std;

class ElementVirtual
{
protected:
    // Элемент видим
    bool _visible = true;
    // Ширина элемента совпадает с шириной родителя
    bool _widthMatchParent = false;
    // Высота элемента совпадает с высотой родителя
    bool _heightMatchParrent = false;

    // Координаты расположения и размеров
    display_position _area{};
    // Родительский элемент
    ElementVirtual *_parent = nullptr;
    // Вертикальное выравнивание
    el_vertical_align _vertAlign = el_vertical_align::EL_ALGN_TOP;

    // Состояние анимации (true в данный момент анимируется)
    bool _isAnimationInProcess = false;
    // Координаты для анимации перемещения
    display_position _flyToArea{};
    // Скорость в пикселях за один кадр анимации по X
    float _stepInFrameX = 1.0;
    // Скорость в пикселях за один кадр анимации по Y
    float _stepInFrameY = 1.0;
    // Аргументы для события завершения анимации
    void *_animArgs = nullptr;
    // Событие завершения анимации
    function<void(ElementVirtual *, void *)> _onAnimationEnd = nullptr;

    // флаги меняющие отображение на страничке
    uint16_t _visualStyleTags = 0x0;

private:
public:
    ElementVirtual() = default;
    virtual ~ElementVirtual() = default;

    explicit ElementVirtual(ElementVirtual *parent, bool widhtMatchParent = false, bool heightMatchParent = false)
    {
        _parent = parent;
        _widthMatchParent = widhtMatchParent;
        _heightMatchParrent = heightMatchParent;
    }

    virtual bool isGroup()
    {
        return false;
    }

    /// @brief Установить флаги меняюще стиль отображения
    /// @param flags флаги можно посмотреть в element_style_tags.h
    virtual ElementVirtual *setVisualStyleFlags(uint16_t flags)
    {
        _visualStyleTags = flags;
        return this;
    }

    virtual uint16_t getVisualStyleFlags()
    {
        return _visualStyleTags;
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

    virtual ElementVirtual *setWidthMatchParent(bool matchParent = true)
    {
        _widthMatchParent = matchParent;
        return this;
    }

    // TODO: реализовать поддержку метода в движке интерфейса
    virtual bool isWidthMatchParent()
    {
        return _widthMatchParent;
    }

    virtual ElementVirtual *setHeightMatchParent(bool matchParent = true)
    {
        _heightMatchParrent = matchParent;
        return this;
    }
    virtual bool isHeightMatchParent()
    {
        return _heightMatchParrent;
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
        if (getParent() != nullptr)
        {
            return _area.leftUp.x + getParent()->getX();
        }
        return _area.leftUp.x;
    }

    virtual ElementVirtual *setX(int x)
    {
        _area.setX(x);
        return this;
    }

    virtual int getY()
    {
        if (getParent() != nullptr)
        {
            return _area.leftUp.y + getParent()->getY();
        }
        return _area.leftUp.y;
    }

    virtual ElementVirtual *setY(int y)
    {
        _area.setY(y);
        return this;
    }

    virtual uint getWidth()
    {
        return _area.getWidth();
    }

    virtual ElementVirtual *setWidth(uint32_t width)
    {
        _area.setWidth(width);
        return this;
    }

    virtual uint getHeight()
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

    /// @brief Старт анимации перемещения с сохранением размеров
    /// для работы этого метода, все элементы с анимацией должны вызывать .nextAnimStep() в событии прорисовки .onDraw()
    /// @param x Конечная по x
    /// @param y конечная по Y
    /// @param frameTime колличество кадров, отведенных на анимацию (время анимации в кадрах)
    /// @param onAnimationEnd событие по завершению анимации
    /// @param animArgs аргументы для передачи в событие
    virtual void flyTo(int x, int y, uint16_t frameTime,
                       function<void(ElementVirtual *, void *)> onAnimationEnd = nullptr, void *animArgs = nullptr)
    {
        _flyToArea = {
            .leftUp = {.x = x, .y = y},
        };
        _flyToArea.setWidth(getWidth());
        _flyToArea.setHeight(getHeight());

        _stepInFrameX = (float)(getX() - x) / frameTime;
        _stepInFrameY = (float)(getY() - y) / frameTime;

        _onAnimationEnd = onAnimationEnd;
        _animArgs = animArgs;

        _isAnimationInProcess = true;
    }

    /// @brief Просчитать положение для анимируемого объекта (вызывать в onDraw)
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
        }
    }

    virtual void onDraw()
    {
        // Стараюсь не использовать это событие для всех элементов,
        // что бы из бежать лишних проверок (например проверку на анимацию для статичных объектов)
        // Используется в: ElScroll
    }

    virtual bool onControl(control_event_type type, void *args)
    {
        return false;
    }

    virtual el_type getElementType() = 0;
};