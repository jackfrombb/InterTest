#pragma once

class ElBattery : public ElProgressBar
{
private:
public:
    ElBattery()
    {

    }

    el_type getElementType() override
    {
        return el_type::EL_TYPE_BATTERY_INDCATOR;
    }
};