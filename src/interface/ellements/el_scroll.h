/**
 * @file el_scroll.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Группа для плавной и быстрой прокрутки на непоместившиеся на дисплее элементы
 * @version 0.1
 * @date 2024-02-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include "ellements_list.h"

class ElScroll : public ElGroup
{
private:
    ElText _testText;
    bool _isOneByOne;                // [Не доделано] Если включен, то добавление элементов будет друг за другом по выбранному направлению
    bool _isHorizontal;              // Вертикальныая или горизонтальная прокрутка
    uint8_t _overscrollPadding = 10; // Величина отступа добавляемая последнему элементу

public:
    ElScroll(bool isOneByOne = false, bool isHorizontal = false)
    {
        _isOneByOne = isOneByOne;
        _isHorizontal = isHorizontal;
    }

    ElScroll *addElement(ElementVirtual *el) override
    {
        // Если элементы нужны выставлять друг за другом и это второй и долше элемент, то суммируем их положения

        if (_isHorizontal)
        {
            if (getElements().size() > 1 && _isOneByOne)
            {
                el->setX(el->getX() + getElement(getElementsCount() - 2)->getX());
            }

            if (getHeight() < el->getHeight())
            {
                setHeight(el->getHeight());
            }
        }
        else
        {
            if (getElements().size() > 1 && _isOneByOne)
            {
                el->setY(el->getY() + getElement(getElementsCount() - 2)->getY());
            }

            if (getWidth() < el->getWidth())
            {
                setWidth(el->getWidth());
            }
        }

        ElGroup::addElement(el);
        return this;
    }

    ElGroup *setOverscrollPadding(uint8_t padding)
    {
        _overscrollPadding = padding;
        return this;
    }

    void scrollTo(ElementVirtual *el)
    {
        if (_isHorizontal)
        {
            int16_t pos = 0 - el->getX();
            setX(pos); // Складываем ширину с положением по x + отступ
        }
        else
        {
            int16_t pos = 0 - el->getY(); // Высчитваем сдвиг
            setY(pos);                    // Складываем высоту с положением по y + отступ
        }
    }

    void smoothScrollTo(ElementVirtual *el)
    {
        if (_isHorizontal)
        {
            int16_t pos = 0 - el->getArea().leftUp.x;
            flyTo(pos, getY(), 10);
        }
        else
        {
            int16_t pos = 0 - el->getArea().leftUp.y;
            // logi::p("Scroll", "Scroll to y: " + String(pos));
            flyTo(getX(), pos, 10);
        }
    }

    void smoothScrollToPosition(uint8_t position)
    {
        smoothScrollTo(getElements()[position]);
    }

    void smoothScrollTo(int xOrY)
    {
        int val = -xOrY;
        if (_isHorizontal)
        {
            flyTo(val, getY(), 10);
        }
        else
        {
            flyTo(getX(), val, 10);
        }
    }

    virtual void onDraw()
    {
        nextAnimStep(); // Для отрисовки плавной прокрутки
    }
};