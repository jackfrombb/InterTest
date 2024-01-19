#pragma once

#include <Arduino.h>
#include "ellement_virtual.h"
#include "displays/display_structs.h"

using namespace std;

template <typename T>
class ElWaveform : public ElementVirtual
{
private:
    T *_points;
    T *_prevPoints;
    OscilVirtual* _oscil;
    uint32_t _pointsLength{};
    uint8_t _sectionCountW = 8;
    uint8_t _sectionCountH = 4;
    float _maxMeasureValue = 4.0;
    function<T*()> _getPointsFunc = nullptr;

public:
    ElWaveform()
    = default;
    
    explicit ElWaveform(T* points)
    {
        _points = points;
    }

    ~ElWaveform(){
        free(_prevPoints);
    }

    void setPointsSource(function<T*()> getPointsFunc, uint32_t lenght){
        _getPointsFunc = getPointsFunc;
        _pointsLength = lenght;
    }

    void setPoints(T *points, uint32_t pointsLength)
    {
        _points = points;
        _pointsLength = pointsLength;
        _prevPoints = (T*) calloc(_pointsLength, sizeof(T));
    }

    T* getPoints(){
        if(_getPointsFunc != nullptr){
            return _getPointsFunc();
        }
        return _points;
    }

    ElWaveform* setOscil(OscilVirtual* oscil){
        _oscil = oscil;

        return this;
    }

    OscilVirtual* getOscil(){
        return _oscil;
    }

    T* getPrevPoints(){
        return _prevPoints;
    }

    ElWaveform* copyPointsToPrev(){
        memcpy(getPrevPoints(), getPoints(), getPointsLength() * sizeof(T));
        return this;
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

    el_type getElementType() override
    {
        return EL_TYPE_WAVEFORM;
    }
};