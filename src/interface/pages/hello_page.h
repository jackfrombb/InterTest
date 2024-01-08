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

    void _progressBarInit(display_resolution resolution)
    {
        // Настройка размера ========================
        int width = resolution.width - 10;
        int height = 15;

        point_t leftUp{
            .x = 5,
            .y = (resolution.height * 0.5) - (height * 0.5),
        };
        point_t rightDown{
            .x = width - 5,
            .y = (resolution.height * 0.5) + (height * 0.5),
        };
        display_area progresBarPos{
            .leftUp = leftUp,
            .rightDown = rightDown,
        };

        // Присоединение переменных
        _progressBar.setProgresPtr(&_progress);
    }

    void _helloTextInit()
    {
        _helloText.setText("Здравствуй");
        int x = ELLEMENT_POSITION_CENTER;
        int y = _progressBar.getArea().rightDown.y + 10;
        point_t leftUp{
            .x = x,
            .y = y,
        };
        _helloText.setArea(display_area{.leftUp = leftUp});
    }

public:
    HelloPage(DisplayVirtual *display) : _progress(0.0)
    {
        display_resolution resolution = display->getResoluton();
        _progressBarInit(resolution);
        _helloTextInit();
        _count = 2;

        _ellements = {
            &_progressBar,
            &_helloText,
        };
    }
    ~HelloPage()
    {
    }
};