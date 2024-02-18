/**
 * @file el_scroll_group.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief
 * Группа для отображения прокручиваемого списка с переиспользованием эллементов и удалением не видимых
 * Отрисовываются видимый элемент +- 1 от него, с помощью getElement функции
 * Для упрощения высчитывания общего размера буду использовать единый размер для элементов
 *
 * @version 0.1
 * @date 2024-02-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

class ElRecycleGroup : public ElGroup
{
private:
    function<ElementVirtual *(uint16_t position)> _getElement = nullptr; // Получить элемент для отрисовки
    function<area_size(uint16_t)> _getElementSize = nullptr;             // Получить размер элемента

    area_size _elSingleSize;
    bool _isSingleSize = false;

    uint16_t _elCount = 0;
    uint16_t _elPosition = 0;

    void _clear(ElementVirtual *el)
    {
        if (el == nullptr)
            return;

        if (el->isGroup())
        {
            for (auto a : ((ElGroup *)el)->getElements())
            {
                _clear(a);
            }
        }

        delete el;
        el = nullptr;
    }

public:
    ElRecycleGroup()
    {
    }

    ~ElRecycleGroup()
    {
        // Очищаем ресурсы
        for (auto el : getElements())
        {
            _clear(el);
        }
    }

    /// @brief Установить единый для всех элементов размер
    /// @param elSize единый размер для каждого элемента
    ElementVirtual *setElementsSingleSize(area_size elSize)
    {
        _isSingleSize = true;
        _elSingleSize = elSize;
        return this;
    }

    /// @brief Установить функцию, позволяющую получить индивидуальный размер элемента
    ElementVirtual *setElementSizeGetter(function<area_size(uint16_t position)> getElementSize)
    {
        _getElementSize = getElementSize;
        return this;
    }

    /// @brief Установить функцию позволяющую получить указатель на элемент
    ElementVirtual *setElementGetter(function<ElementVirtual *(uint16_t position)> getElement, uint16_t elCount)
    {
        _elCount = elCount;
        _getElement = getElement;
        auto el = getElement(_elPosition);
        addElement(el);
        return this;
    }

    void onDraw() override
    {
        for (auto el : getElements())
        {
            el->nextAnimStep();
        }
    }

    el_type getElementType()
    {
        return el_type::EL_TYPE_GROUP;
    }
};