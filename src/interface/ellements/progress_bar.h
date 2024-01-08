#pragma once
#include "ellement_virtual.h"

class ElProgressBar : public EllementVirtual
{
private:
    float *_progress;

public:
    ElProgressBar() : _progress(new float()) {}
    ~ElProgressBar()
    {
        if (_progress != nullptr)
            delete _progress;
    }

    void setProgress(float progress)
    {
        if(progress>1.0) progress = 1.0;
        _progress = new float(progress);
    }

    void setProgresPtr(float *progress)
    {
        _progress = progress;
    }

    int8_t getProgress()
    {
        if (_progress != nullptr)
            return *_progress;
        else
            return 0;
    }
};