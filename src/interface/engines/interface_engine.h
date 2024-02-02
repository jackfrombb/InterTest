#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"

class InterfaceEngineVirtual
{
protected:
    InterfaceEngineVirtual() = default;
    virtual ~InterfaceEngineVirtual() = default;

    virtual void _onStartDraw() {}
    virtual void _onEndDraw() {}

    virtual void drawProgressBar(ElProgressBar *progressBar) = 0;
    virtual void drawButton(ElTextButton *button) = 0;
    virtual void drawWaveform(ElWaveform *waveform) = 0;
    virtual void drawText(ElText *text) = 0;
    virtual void drawCenteredGroup(ElCenteredGroup *group) = 0;
    virtual void drawLine(ElLine *line) = 0;

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
            }
        else
            logi::p("EngineVirtual", "Try to draw empty element");
    }

private:
public:
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
};