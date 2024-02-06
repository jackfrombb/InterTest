#include "ellement_virtual.h"

class ElDisplayTest : public ElementVirtual
{

public:
    el_type getElementType() override
    {
        return el_type::EL_TYPE_DISPLAY_TEST;
    }
};