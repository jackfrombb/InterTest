#pragma once

#include "el_group.h"

class ElCenteredGroup : public ElGroup
{
protected:
    uint16_t _articularWidth;

public:
    ElCenteredGroup()
    {
        _articularWidth = 0;
    }

    ElGroup *addElement(ElementVirtual *element) override
    {
        ElGroup::addElement(element);
        return this;
    }

    el_type getElementType() override
    {
        return el_type::EL_TYPE_CENTERED_GROUP;
    }
};