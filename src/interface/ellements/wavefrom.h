#pragma once

#include <Arduino.h>
#include "ellement_virtual.h"
#include "displays/display_structs.h"

template <typename T>
class ElWaveform : public EllementVirtual
{
private:
    T *_points;
    uint32_t _pointsLength;
    uint8_t _sectionCountW = 8;
    uint8_t _sectionCountH = 4;
    float _maxMeasureValue = 4.0;

public:
    ElWaveform()
    {
        
    } 
    
    ElWaveform(T* points)
    {
        _points = points;
    }
    ~ElWaveform()
    {
    }

    void setPoints(T *points, uint32_t pointsLength)
    {
        _points = points;
        _pointsLength = pointsLength;
    }

    T* getPoints(){
        return _points;
    }

    uint32_t getPointsLength(){
        return _pointsLength;
    }

    void setWidthSectionsCount(uint8_t count){
        _sectionCountW = count;
    } 
    
    void setHeightSectionsCount(uint8_t count){
        _sectionCountH = count;
    }

    void setMaxMeasureValue(float val){
        _maxMeasureValue = val;
    }

    float getMaxMeasureValue(){
        return _maxMeasureValue;
    }

    uint8_t getWidthSectionsCount(){
        return _sectionCountW;
    }

    uint8_t getHeightSectionsCount(){
        return _sectionCountH;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_WAVEFORM;
    }
};