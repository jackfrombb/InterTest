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

    virtual ElGroup *addEllement(EllementVirtual *ellement)
    {
        _articularWidth += ellement->getWidth();
        ElGroup::addEllement(ellement);
        return this;
    }

    uint16_t getArticularWidth()
    {
        return _articularWidth;
    }

    virtual el_type getEllementType()
    {
        return el_type::EL_TYPE_CENTERED_GROUP;
    }
};