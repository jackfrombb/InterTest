#pragma once

#include "interface/pages/views/page_view.h"
#include "interface/pages/page_virtual.h"

class StartPageView : public PageView
{
private:
    pages_list _focusPage = pages_list::PAGE_LIST_OSCIL;

    String *_currentTitle;

    ElText _prev;
    ElText _current;
    ElText _next;

    function<void(pages_list)> _onPageSelected;

    void _initPoints()
    {
        _current
            .setCalculatedText([this]{
                return pages_list_getName(_focusPage);
            })
            ->setTextSize(el_text_size::EL_TEXT_SIZE_MIDDLE)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_LEFT)
            ->setX(0) // на весь экран
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) // Вертикально по центру
            ;

        _currentTitle = _current.getTextPtr();
    }

public:
    StartPageView(DisplayVirtual *display, function<void(pages_list)> onPageSelected) : PageView(display)
    {
        _onPageSelected = onPageSelected;

        _initPoints();

        addElement(&_current);
    }

    ~StartPageView() = default;

    void onControlEvent(control_event_type eventType) override
    {
        switch (eventType)
        {
        case control_event_type::PRESS_OK:
            break;
        case control_event_type::PRESS_LEFT:
            _focusPage = (pages_list)range(((int)_focusPage - 1), 0, pages_list::pagesCount-1);
            //*_currentTitle = pages_list_getName(_focusPage);
            break;
        case control_event_type::PRESS_RIGHT:
            _focusPage = (pages_list)range(((int)_focusPage + 1), 0, pages_list::pagesCount-1);
            //*_currentTitle = pages_list_getName(_focusPage);
            break;
        }
    }
};