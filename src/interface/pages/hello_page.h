#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"
#include "displays/display_helper.h"
#include "displays/display_virtual.h"

class HelloPage : public InterfacePageVirtual
{
private:
    float* _progress;
    ElProgressBar _progressBar;
    ElText _helloText;

    void _progressBarInit(display_resolution resolution)
    {
        // Настройка размера ========================
        int width = resolution.width - 10;
        int height = 10;

        point_t leftUp{
            .x = 5,
            .y = (int) (((float)resolution.height * 0.5) - ((float)height * 0.5)),
        };
        
        point_t rightDown{
            .x = width,
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
    HelloPage(DisplayVirtual *display, float* progress) : InterfacePageVirtual(display)
    {
        _progress = progress;

        display_resolution resolution = display->getResoluton();
        _progressBarInit(resolution);
        _helloTextInit();

        _ellements = {
            &_progressBar,
            //&_helloText,
        };
    }

    ~HelloPage()
    {
    }
};