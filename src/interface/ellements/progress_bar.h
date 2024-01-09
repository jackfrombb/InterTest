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
        if (progress > 1.0)
            progress = 1.0;
        _progress = new float(progress);
    }

    void setProgresPtr(float *progress)
    {
        _progress = progress;
    }

    float getProgress()
    {
        if (_progress != nullptr)
            return min<float>(*_progress, 1.0);
        else
            return 1.0; // Если ссылка вникуда, то, вероятнее всего, загрузка завершилась и переенная стерта из кучи
    }

    virtual el_type getEllementType()
    {
        return EL_TYPE_PROGRESS_BAR;
    }
};