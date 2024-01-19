#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"

class InterfaceEngineVirtual
{
protected:
    InterfaceEngineVirtual() {}

    virtual void _onStartDraw() {}
    virtual void _onEndDraw() {}

    virtual void drawProgressBar(ElProgressBar *progressBar) = 0;
    virtual void drawButton(ElTextButton *button) = 0;
    virtual void drawWaveform(ElWaveform<uint16_t> *waveform) = 0;
    virtual void drawText(ElText *text) = 0;
    virtual void drawCenteredGroup(ElCenteredGroup *group) = 0;

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
        switch (el->getElementType())
        {
        case el_type::EL_TYPE_BUTTON:
            drawButton((ElTextButton *)el);
            break;

        case el_type::EL_TYPE_WAVEFORM:
            drawWaveform((ElWaveform<uint16_t>*)el);
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
        }
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
};