#pragma once
//#include "interface/pages/views/page_view.h"

class HelloPageView : public PageView
{
protected:
    void _progressBarInit()
    {
        display_resolution resolution = _display->getResolution();

        // Настройка размера ========================
        int width = resolution.width - 10;
        int height = 10;

        // На четверть от низа
        int y = (int)(((resolution.height - (float)resolution.height * 0.25)) - ((float)height * 0.5));

        point_t leftUp{
            .x = 5,
            .y = y,
        };

        point_t rightDown{
            .x = width + leftUp.x,
            .y = leftUp.y + height,
        };

        display_position progresBarPos{
            .leftUp = leftUp,
            .rightDown = rightDown,
        };

        // Присоединение переменных
        _progressBar.setArea(progresBarPos);
        _progressBar.setProgressPtr(_progress);
    }

    void _helloTextInit()
    {
        _helloText.setText("InterTest")
            ->setTextSize(EL_TEXT_SIZE_MIDDLE);

        // Выше чем прогресс загрузки ни 10px
        int y = _progressBar.getArea().leftUp.y - 20;

        _helloText.setPosition(0, y);
        _helloText.setWidth(_display->getWidth());
        _helloText.setHeight((uint32_t)(_display->getHeight()*0.5));
        _helloText.setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER);

    }

    void _versionTextInit()
    {
        _versionText.setText("ver: " + String(APP_VERSION))
            ->setTextSize(EL_TEXT_SIZE_SUPER_SMALL);

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

        addElement(&_progressBar)->addElement(&_versionText)->addElement(&_helloText);
    }
};