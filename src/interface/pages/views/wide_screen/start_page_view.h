#pragma once

#include "interface/pages/views/page_view.h"
#include "interface/pages/page_virtual.h"

#define TEST_ANIM_ENABLE

class StartPageView : public PageView
{
private:
    pages_list _focusPage = pages_list::PAGE_LIST_OSCIL;

    String *_currentTitle;
    String *_nextTitle;
    String *_prevTitle;

    ElText _current;
    ElText _prev;
    ElText _next;

    function<void(pages_list)> _onPageSelected;

#ifdef TEST_ANIM_ENABLE
    display_position _prevPosDef;
    display_position _nexPosDef;
    display_position _curPosDef;

    int8_t lastDir = 0;
#endif

    void _initPoints()
    {
        _current
            .setCalculatedText([this]
                               { return pages_list_getName(_focusPage); })
            ->setX(0) // на весь экран
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) // Вертикально по центру
            ;

        _prev.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage - 1)); })
            ->setX(-(_display->getWidth()))
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) //
            ;

        _next.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage + 1)); })
            ->setX(_display->getWidth())
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) //
            ;

#ifdef TEST_ANIM_ENABLE
        _currentTitle = _current.getTextPtr();
        _nextTitle = _next.getTextPtr();
        _prevTitle = _prev.getTextPtr();

        _prevPosDef = _prev.getArea();
        _nexPosDef = _next.getArea();
        _curPosDef = _current.getArea();
#endif
    }

#ifdef TEST_ANIM_ENABLE
    void _spinPoints(int8_t dir)
    {
        if ((_current.isAnimationInProcess() || _next.isAnimationInProcess() || _prev.isAnimationInProcess()) || dir == 0)
            return;

        lastDir = dir;

        int frameTime = 7;

        auto onAniationEnd = [this](ElementVirtual *el, void *args)
        {
            _focusPage = (pages_list)range(((int)_focusPage + (1 * lastDir)), 0, pages_list::pagesCount - 1);
            logi::p("StartPageView", "animation ends focus on : " + String(_focusPage));

            _prev.setArea(_prevPosDef);
            _next.setArea(_nexPosDef);
            _current.setArea(_curPosDef);
        };

        if (dir > 0)
        {
            _next.flyTo(0, _next.getY(), frameTime - 1);
        }
        else
        {
            _prev.flyTo(0, _prev.getY(), frameTime - 1);
        }

        _current.flyTo(_display->getWidth() * (dir * -1), _current.getY(), frameTime, onAniationEnd, &_curPosDef);

        // logi::p("StartPageView", "_spinPoints dir: " + String(dir));
    }

#endif

public:
    StartPageView(DisplayVirtual *display, function<void(pages_list)> onPageSelected) : PageView(display)
    {
        _onPageSelected = onPageSelected;

        _initPoints();

        addElement(&_current)->addElement(&_next)->addElement(&_prev);
    }

    ~StartPageView() = default;

    void onControlEvent(control_event_type eventType) override
    {
        switch (eventType) // Открыть страничку
        {
        case control_event_type::PRESS_OK:
            _onPageSelected(_focusPage);
            break;

        case control_event_type::PRESS_LEFT: // Поворот или кнопка влево
#ifdef TEST_ANIM_ENABLE
            if ((int)_focusPage != 0)
                _spinPoints(-1);
#else
            _focusPage = (pages_list)range(((int)_focusPage - 1), 0, pages_list::pagesCount - 1);
#endif

            break;

        case control_event_type::PRESS_RIGHT: // Поворот или кнопка вправо
#ifdef TEST_ANIM_ENABLE
            if ((int)_focusPage != pages_list::pagesCount - 1)
                _spinPoints(+1);
#else
            _focusPage = (pages_list)range(((int)_focusPage + 1), 0, pages_list::pagesCount - 1);
#endif
            break;
        }
    }

    void onDraw() override
    {
        // Подрубаем анимацию для пунктов
        _next.nextAnimStep();
        _prev.nextAnimStep();
        _current.nextAnimStep();
    }
};