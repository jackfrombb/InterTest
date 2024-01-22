#pragma once
#include "ellement_virtual.h"

class ElLine : public ElementVirtual
{
    private:
    public:

    el_type getElementType() override {
        return el_type::EL_TYPE_LINE;
    }
};