#pragma once

#include <Arduino.h>
#include "ellement_virtual.h"
#include "displays/display_structs.h"

template <typename T>
class ElWaveform : public EllementVirtual
{
private:
    T *_points;
    int32_t _pointsLength;

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

    void setPoints(T *points, int32_t pointsLength)
    {
        _points = points;
        _pointsLength = pointsLength;
    }

    T* getPoints(){
        return _points;
    }

    int32_t getPointsLength(){
        return _pointsLength;
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_WAVEFORM;
    }
};