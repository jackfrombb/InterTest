#pragma once
#include "ellement_virtual.h"
#include "helpers.h"

class ElProgressBar : public ElementVirtual
{
private:
    float *_progress;

public:
    ElProgressBar()
    {
    }
    ~ElProgressBar()
    {
    }

    void setProgressPtr(float *progress)
    {
        _progress = progress;
    }

    float getProgress()
    {
        if (_progress != nullptr)
            return rangeV2<float>(*_progress, 0.0, 1.0);
        else
            return 1.0; // Если ссылка вникуда, то, вероятнее всего, загрузка завершилась и переенная стерта из кучи
    }

    el_type getElementType() override
    {
        return EL_TYPE_PROGRESS_BAR;
    }
};