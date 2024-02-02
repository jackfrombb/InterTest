#pragma once

#include "el_group.h"

class ElCenteredGroup : public ElGroup
{
protected:
    uint16_t _articularWidth;
    bool _drawFrame = false; // Флаг необходимости рисовать рамку вокруг группы

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

    bool isNeedDrawFrameAround()
    {
        return _drawFrame;
    }

    ElCenteredGroup *setNeedDrawFrame(bool need)
    {
        _drawFrame = need;
        return this;
    }
};