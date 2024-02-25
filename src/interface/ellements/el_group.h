#pragma once
#include "ellement_virtual.h"
#include <vector>

class ElGroup : public ElementVirtual
{
private:
    std::vector<ElementVirtual *> _elements;

protected:
public:
    ElGroup(/* args */) = default;

    ~ElGroup() = default;

    virtual int8_t getElementsCount()
    {
        return (int8_t)_elements.size();
    }

    virtual ElementVirtual *getElement(uint8_t num)
    {
        return _elements[num];
    }

    virtual ElGroup *addElement(ElementVirtual *element)
    {
        element->setParent(this);
        _elements.push_back(element);
        return this;
    }

    bool isGroup() override
    {
        return true;
    }

    std::vector<ElementVirtual *> getElements()
    {
        return _elements;
    }

    el_type getElementType() override { return el_type::EL_TYPE_GROUP; }
};