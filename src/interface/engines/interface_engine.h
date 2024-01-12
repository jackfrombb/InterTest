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
        for (auto el : group->getEllements())
        {
            if (!el->isVisible())
                continue;

            drawElement(el);
        }
    }

    virtual void drawElement(EllementVirtual *el)
    {
        switch (el->getEllementType())
        {
        case el_type::EL_TYPE_BUTTON:
            drawButton((ElTextButton *)el);
            break;

        case el_type::EL_TYPE_WAVEFORM:
            drawWaveform((ElWaveform<uint16_t> *)el);
            break;

        case el_type::EL_TYPE_TEXT:
            drawText((ElText *)el);
            break;

        case el_type::EL_TYPE_PROGRESS_BAR:
            drawProgressBar((ElProgressBar *)el);
            break;

        case el_type::EL_TYPE_GROUP:
            //Serial.println("Draw group");
            drawGroup((ElGroup *)el);
            break;

        case el_type::EL_TYPE_CENTERED_GROUP:
            //Serial.println("Draw centered");
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