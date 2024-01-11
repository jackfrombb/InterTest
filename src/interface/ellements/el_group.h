#pragma once
#include "ellement_virtual.h"
#include <vector>

class ElGroup : public EllementVirtual
{
private:
    std::vector<EllementVirtual *> _ellements;
protected:
public:
    ElGroup(/* args */)
    {
    }

    ~ElGroup()
    {
    }

    virtual int8_t getEllementsCount()
    {
        return _ellements.size();
    }

    virtual EllementVirtual *getEllement(uint8_t num)
    {
        return _ellements[num];
    }

    virtual ElGroup *addEllement(EllementVirtual *ellement)
    {
        ellement->setParent(this);
        _ellements.push_back(ellement);
        return this;
    }

    virtual bool isGroup()
    {
        return true;
    }

    std::vector<EllementVirtual *> getEllements()
    {
        return _ellements;
    }

    virtual el_type getEllementType() { return el_type::EL_TYPE_GROUP; };
};