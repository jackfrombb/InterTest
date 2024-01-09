#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"
#include "displays/display_helper.h"
#include "displays/display_virtual.h"

class HelloPage : public InterfacePageVirtual
{
private:
    float _progress;
    ElProgressBar _progressBar;
    ElText _helloText;
    ElText _versionText;

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
        _progressBar.setProgresPtr(&_progress);
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
    HelloPage(DisplayVirtual *display) : InterfacePageVirtual(display), _progress(0.0)
    {
        _progressBarInit();
        _versionTextInit();
        _helloTextInit();

        _ellements = {
            &_progressBar,
            &_versionText,
            &_helloText,
        };
    }

    ~HelloPage()
    {
    }

    float *getProgressPtr()
    {
        return &_progress;
    }
};