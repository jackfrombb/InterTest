#pragma once
#include "ellement_virtual.h"
#include <vector>

class ElGroup : public EllementVirtual
{
protected:
    std::vector<EllementVirtual *> _ellements;

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

    virtual void addEllement(EllementVirtual *ellement)
    {
        _ellements.push_back(ellement);
    }

    
    virtual el_type getEllementType() { return el_type::EL_TYPE_GROUP; };

    virtual bool isGroup(){ 
        return true;
    }

};