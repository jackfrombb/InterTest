#pragma once

#include <Arduino.h>
#include "ellement_virtual.h"
#include "displays/display_structs.h"

using namespace std;

class ElWaveform : public ElementVirtual
{
private:
    adc_measures_t _measures;
    uint32_t _pointsLength{};
    uint8_t _sectionCountW = 8;
    uint8_t _sectionCountH = 4;
    float _maxMeasureValue = 4.0;
    bool _drawDots;
    bool _drawBackGround;
    function<adc_measures_t()> _getMeasuresFunc = nullptr;

public:
    ElWaveform() = default;

    ~ElWaveform() = default;

    void setPointsSource(function<adc_measures_t()> getMeasuresFunc)
    {
        _getMeasuresFunc = getMeasuresFunc;
        _drawDots = false; //AppData::getBool("waveformDots", false);
        _drawBackGround = true;
    }

    void setPoints(adc_measures_t measures)
    {
        _measures = measures;
    }

    adc_measures_t* getMeasuresPtr(){
        return &_measures;
    }

    adc_measures_t getMeasures()
    {
        if (_getMeasuresFunc != nullptr)
        {
            return _getMeasuresFunc();
        }
        return _measures;
    }

    bool isNeedDrawBackground(){
        return _drawBackGround;
    }

    bool isNeedDrawBackDots()
    {
        return _drawDots;
    }

    ElWaveform *setNeedDrawBackDots(bool isNeed)
    {
        _drawDots = isNeed;
        return this;
    }

    ElWaveform *setNeedDrawBackground(bool isNeed)
    {
        _drawBackGround = isNeed;
        return this;
    }

    uint32_t getPointsLength()
    {
        return _pointsLength;
    }

    void setWidthSectionsCount(uint8_t count)
    {
        _sectionCountW = count;
    }

    void setHeightSectionsCount(uint8_t count)
    {
        _sectionCountH = count;
    }

    void setMaxMeasureValue(float val)
    {
        _maxMeasureValue = val;
    }

    float getMaxMeasureValue()
    {
        return _maxMeasureValue;
    }

    uint8_t getWidthSectionsCount()
    {
        return _sectionCountW;
    }

    uint8_t getHeightSectionsCount()
    {
        return _sectionCountH;
    }

    el_type getElementType() override
    {
        return EL_TYPE_WAVEFORM;
    }
};