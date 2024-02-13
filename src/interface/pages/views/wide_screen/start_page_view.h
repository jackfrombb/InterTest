#pragma once

// #include "interface/pages/views/page_view.h"
// #include "interface/pages/page_virtual.h"
// #include "app_data.h"

#define TEST_ANIM_ENABLE

class StartPageView : public PageView
{
private:
    // Выбранный на данный момент пункт (стандартно осцилограф, но в настройках сохраняется последний выбранный)
    pages_list _focusPage = pages_list::PAGE_LIST_OSCIL;

    // Ссылка на текстовое поле текущего пункта
    String *_currentTitle;
    // Ссылка на текстовое поле следующего пункта
    String *_nextTitle;
    // Ссылка на текстовое поле предыдущего пункта
    String *_prevTitle;

    // Текущий пункт
    ElText _current;
    // Предыдущий пункт
    ElText _prev;
    // Последующий пункт
    ElText _next;
    // Поле отображающее текущую прокрутку
    ElScrollBar _scrollBar;

    // Просто тестовый элемент для проверки дисплея, удалю или перенесу по необходимости
    ElDisplayTest _displayTest;

    // Событие выбора странички пользователем
    function<void(pages_list)> _onPageSelected;

#ifdef TEST_ANIM_ENABLE
    // Позиция по умолчанию для предыдущего пункта
    display_position _prevPosDef;
    // Позиция по умолчанию для следующего пункта
    display_position _nexPosDef;
    // Позиция по умолчанию для текущего пункта
    display_position _curPosDef;

    int8_t lastDir = 0;
#endif
    void _scrollFocus(int8_t direction)
    {
        _focusPage = (pages_list)range(((int)_focusPage + (1 * direction)), 0, pages_list::pagesCount - 1);
        _calculateScrollPosition();
    }

    void _initPoints()
    {
        _current
            .setCalculatedText([this]
                               { return pages_list_getName(_focusPage); })
            //->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setX(10) // на весь экран
            ->setY(0)
            ->setWidth(_display->getWidth() - 10)
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) // Вертикально по центру
            ;

        _prev.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage - 1)); })
            ->setX(-(_display->getWidth() + 10))
            ->setY(0)
            ->setWidth(_display->getWidth() - 10)
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) //
            ;

        _next.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage + 1)); })
            ->setX(_display->getWidth() + 10)
            ->setY(0)
            ->setWidth(_display->getWidth() - 10)
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

    void _initScrollBar()
    {
        uint16_t scrollBarWidth = _display->getWidth() - (_display->getWidth() >> 2);
        uint16_t scrollBarHeight = 6;

        _scrollBar
            .setX((_display->getWidth() - scrollBarWidth) >> 1)
            ->setY(_display->getHeight() - (scrollBarHeight + 2))
            ->setWidth(scrollBarWidth)
            ->setHeight(scrollBarHeight);
    }

    void _calculateScrollPosition()
    {
        _scrollBar.setScrollPosition(_display->getWidth() * (pages_list::pagesCount - 1), // Общая ширина всех пунктов
                                     _focusPage * _display->getWidth(),                   // Текущее положение по x
                                     _display->getWidth());                               // Текущая отображаемая ширина
    }

#ifdef TEST_ANIM_ENABLE
    void _spinPoints(int8_t dir)
    {
        if ((_current.isAnimationInProcess() || _next.isAnimationInProcess() || _prev.isAnimationInProcess()))
        {
            // Если анимация предыдущего перелистывания еще не закончилась,
            //  то переводим фокус без анимации, что бы избежать заторможенности
            _scrollFocus(1 * dir);
            return;
        }

        lastDir = dir;

        int frameTime = 7;

        auto onAniationEnd = [this](ElementVirtual *el, void *args)
        {
            _scrollFocus(1 * lastDir);
            // logi::p("StartPageView", "animation ends focus on : " + String(_focusPage));

            _prev.setArea(_prevPosDef);
            _next.setArea(_nexPosDef);
            _current.setArea(_curPosDef);
        };

        if (dir > 0)
        {
            _next.flyTo(_curPosDef.getX(), _next.getY(), frameTime - 1);
        }
        else
        {
            _prev.flyTo(_curPosDef.getX(), _prev.getY(), frameTime - 1);
        }

        _current.flyTo(_display->getWidth() * (dir * -1), _current.getY(), frameTime, onAniationEnd, &_curPosDef);

        // logi::p("StartPageView", "_spinPoints dir: " + String(dir));
    }
#endif

public:
    StartPageView(DisplayVirtual *display, function<void(pages_list)> onPageSelected) : PageView(display)
    {
        _onPageSelected = onPageSelected;

        if (AppData::exist("focusPage"))
        {
            _focusPage = (pages_list)AppData::getInt("focusPage", 0);
            logi::p("StartPageView", "focusPage val exist: " + String(_focusPage));
        }

        _initPoints();
        _initScrollBar();
        _calculateScrollPosition();

        _displayTest.setArea({.leftUp = {.x = 0, .y = 0}, .rightDown = {.x = _display->getWidth(), .y = _display->getHeight()}});

        addElement(&_current)
            ->addElement(&_next)
            ->addElement(&_prev)
            ->addElement(&_scrollBar)
            ->addElement(&_displayTest);
    }

    ~StartPageView() = default;

    bool onControlEvent(control_event_type eventType) override
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
            _scrollFocus(-1);
#endif

            break;

        case control_event_type::PRESS_RIGHT: // Поворот или кнопка вправо
#ifdef TEST_ANIM_ENABLE
            if ((int)_focusPage != pages_list::pagesCount - 1)
                _spinPoints(+1);
#else
            _scrollFocus(+1);
#endif
            break;
        }

        return true;
    }

    void onDraw() override
    {
        // Подрубаем анимацию для пунктов
        _next.nextAnimStep();
        _prev.nextAnimStep();
        _current.nextAnimStep();
    }

    void onClose() override
    {
        AppData::saveInt("focusPage", _focusPage);
        AppData::flush();
    }
};