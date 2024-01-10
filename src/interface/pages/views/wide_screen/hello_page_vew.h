#pragma once
#include "interface/pages/views/page_view.h"

class HelloPageView : public PageView
{
protected:
    virtual void _progressBarInit()
    {
        display_resolution resolution = _display->getResoluton();

        // Настройка размера ========================
        int width = resolution.width - 10;
        int height = 10;

        // На четверть от низа
        int y = ((resolution.height - (float)resolution.height * 0.25)) - ((float)height * 0.5);

        point_t leftUp{
            .x = 5,
            .y = y,
        };

        point_t rightDown{
            .x = width + leftUp.x,
            .y = leftUp.y + height,
        };

        display_area progresBarPos{
            .leftUp = leftUp,
            .rightDown = rightDown,
        };

        // Присоединение переменных
        _progressBar.setArea(progresBarPos);
        _progressBar.setProgresPtr(_progress);
    }

    virtual void _helloTextInit()
    {
        _helloText.setTextSize(EL_SIZE_MIDDLE);
        _helloText.setText("InterTest");
        // По центру экрана
        int x = ELLEMENT_POSITION_CENTER;
        // Выше чем прогресс загрузки ни 10px
        int y = _progressBar.getArea().leftUp.y - 20;

        _helloText.setPosition(x, y);
    }

    virtual void _versionTextInit()
    {
        _versionText.setText("ver: " + String(APP_VERSION));
        _versionText.setTextSize(EL_SIZE_SUPER_SMALL);

        int x = 0;
        int y = 0;

        _versionText.setPosition(x, y);
    }

public:
    ElProgressBar _progressBar;
    ElText _helloText;
    ElText _versionText;

    float *_progress;

    HelloPageView(DisplayVirtual *display, float *progress) : PageView(display)
    {
        _progress = progress;

        _progressBarInit();
        _versionTextInit();
        _helloTextInit();

        _ellements = {
            &_progressBar,
            &_versionText,
            &_helloText,
        };
    }
};