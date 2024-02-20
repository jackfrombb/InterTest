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
        if (!group->isVisible())
            return;

        group->onDraw();

        for (auto el : group->getElements())
        {
            if (!el->isVisible())
                continue;

            el->onDraw();
            drawElement(el);
        }
    }

    virtual void drawElement(ElementVirtual *el)
    {
        if (el != nullptr)
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
        drawGroup(group);
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