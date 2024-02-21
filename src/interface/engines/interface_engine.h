/**
 * @file interface_engine.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Виртуальный класс, представляющий частично абстрактные функции отрисовки элементов на дисплее
 * - Главный метод отрисовки страницы - drawPage, потому что там взываются события начала и конца отрисовки страницы.
 * - Главный метод отрисовки для элементов - drawElement, там так же вызываются события, 
 *   идет проверка и потом уже выбор метода отрисовки
 * - Отрисовка идет в замкнутых на себя методах/функциях, которые берут страничку как группу и начинают поочерёдно идти по элементам
 *   пока не дойдет последнего, пропуская скрытые и nullptr элементы
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once
// #include "interface/ellements/ellements_list.h"
//  #include "interface/pages/page_virtual.h"

class InterfaceEngineVirtual
{
protected:
    bool inverImg = false;

    // Событие вызывается перед стартом прорисовки
    virtual void _onStartDraw() {}
    // Событие вызывается после прорисовки
    virtual void _onEndDraw() {}

    /* Все методы возвращают фактическое полоение на дисплее */

    virtual void drawProgressBar(ElProgressBar *progressBar) = 0;
    virtual void drawWaveform(ElWaveform *waveform) = 0;
    /// @brief Отрисовать текст
    /// @return реальное положение на экране после отрисовки (при создании не всегдна можно заранее знать реальную ширину и высоту)
    virtual display_position drawText(ElText *text) = 0;
    /// @brief Отрисовать текстовую кнопку
    /// @return реальное положение на экране после отрисовки (при создании не всегдна можно заранее знать реальную ширину и высоту)
    virtual display_position drawButton(ElTextButton *button) = 0;
    virtual void drawCenteredGroup(ElCenteredGroup *group) = 0;
    virtual void drawLine(ElLine *line) = 0;
    virtual void drawDisplayTest(ElDisplayTest *displayTest) {}
    virtual void drawBatteryIndicr(ElBattery *batteryIndcr) = 0;
    virtual void drawScrollbar(ElScrollBar *scrollbar) = 0;

    virtual void drawGroup(ElGroup *group)
    {
        for (auto el : group->getElements())
        {
            drawElement(el);
        }
    }

    virtual void drawElement(ElementVirtual *el)
    {
        if (el != nullptr)
        {
            if (!el->isVisible())
                return;

            el->onDraw();

            switch (el->getElementType())
            {
            case el_type::EL_TYPE_BUTTON:
                drawButton((ElTextButton *)el);
                break;

            case el_type::EL_TYPE_WAVEFORM:
                drawWaveform((ElWaveform *)el);
                break;

            case el_type::EL_TYPE_TEXT:
                drawText((ElText *)el);
                break;

            case el_type::EL_TYPE_PROGRESS_BAR:
                drawProgressBar((ElProgressBar *)el);
                break;

            case el_type::EL_TYPE_GROUP:
                drawGroup((ElGroup *)el);
                break;

            case el_type::EL_TYPE_CENTERED_GROUP:
                drawCenteredGroup((ElCenteredGroup *)el);
                break;

            case el_type::EL_TYPE_LINE:
                drawLine((ElLine *)el);
                break;

            case el_type::EL_TYPE_DISPLAY_TEST:
                drawDisplayTest((ElDisplayTest *)el);
                break;

            case el_type::EL_TYPE_BATTERY_INDCATOR:
                drawBatteryIndicr((ElBattery *)el);
                break;

            case el_type::EL_TYPE_SCROLLBAR:
                drawScrollbar((ElScrollBar *)el);
                break;
            }
        }
        else
            logi::p("EngineVirtual", "Try to draw empty element");
    }

private:
public:
    InterfaceEngineVirtual() = default;
    virtual ~InterfaceEngineVirtual() {}

    /// @brief Отобразить страницу на экране
    /// @param page
    void drawPage(ElGroup *group)
    {
        _onStartDraw();
        drawElement(group);
        _onEndDraw();
    }

    virtual uint8_t getMaxTextWidth(el_text_size textSize) = 0;
    virtual uint8_t getMaxTextHeight(el_text_size textSize) = 0;

    /// @brief Инвертировать изображение в буфере
    virtual void invertImg(bool invert)
    {
        inverImg = invert;
    }
};