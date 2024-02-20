#pragma once

// #include "interface/pages/views/page_view.h"
// #include "interface/pages/page_virtual.h"
// #include "app_data.h"

#define TEST_ANIM_ENABLE

class StartPageView : public PageView
{
private:
    const uint8_t style_PagePadding = 10; // Внешние отступы на страничке

    // Выбранный на данный момент пункт (стандартно осцилограф, но в настройках сохраняется последний выбранный)
    pages_list _focusPage = pages_list::PAGE_LIST_OSCIL;

    // Ссылка на текстовое поле текущего пункта (для передачи в управление из представления в контроллер странички)
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
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
            ->setX(0) // на весь экран
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) // Вертикально по центру
            ;

        _prev.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage - 1)); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
            ->setX(-(_display->getWidth()))
            ->setY(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getHeight())
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER) //
            ;

        _next.setCalculatedText([this]
                                { return pages_list_getName((pages_list)((int)_focusPage + 1)); })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)

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

    void _initScrollBar()
    {
        uint16_t scrollBarWidth = _display->getWidth() - (style_PagePadding << 1);
        uint16_t scrollBarHeight = 4;

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
        // Если анимация предыдущего перелистывания еще не закончилась,
        //  то переводим фокус без анимации, что бы избежать заторможенности
        if ((_current.isAnimationInProcess() || _next.isAnimationInProcess() || _prev.isAnimationInProcess()))
        {
            _scrollFocus(1 * dir);
            return;
        }

        // Сохраняем направление перемотки (лево -1, право +1)
        lastDir = dir;

        // Время анимации в кадрах
        int frameTime = 7;

        // Лямбда функция - событие по завершению анимации
        auto onAniationEnd = [this](ElementVirtual *el, void *args)
        {
            // Переводим фокус на выбранный пользователем
            _scrollFocus(1 * lastDir);

            // Сбрасываем положения эл-тов на стартовые
            _prev.setArea(_prevPosDef);
            _next.setArea(_nexPosDef);
            _current.setArea(_curPosDef);
        };

        // порядок старта анимации важен. 
        // эл-нт, на котором висит событие завершения анимации, должен закончить её последним
        if (dir > 0)
        {
            _next.flyTo(_curPosDef.getX(), _next.getY(), frameTime);
        }
        else
        {
            _prev.flyTo(_curPosDef.getX(), _prev.getY(), frameTime);
        }

        _current.flyTo(_display->getWidth() * (dir * -1), _current.getY(), frameTime, onAniationEnd, &_curPosDef);
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