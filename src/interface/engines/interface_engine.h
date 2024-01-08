#pragma once
#include "interface/ellements/ellements_list.h"
#include "interface/pages/page_virtual.h"

class InterfaceEngineVirtual
{
private:
public:
    /// @brief Отобразить страницу на экране
    /// @param page
    virtual void drawPage(InterfacePageVirtual page)
    {
        for (int i = 0; i < page.getSize(); i++)
        {
            switch (page.getEllement(i)->getEllementType())
            {
            case EL_TYPE_BUTTON:
                drawButton((ElTextButton *)page.getEllement(i));
                break;

            case EL_TYPE_WAVEFORM:
                drawWaveform((ElWaveform<uint16_t> *)page.getEllement(i));
                break;

            case EL_TYPE_TEXT:
                drawText((ElText *)page.getEllement(i));
                break;

            case EL_TYPE_PROGRESS_BAR:
                drawProgressBar((ElProgressBar *)page.getEllement(i));
                break;
            }
        }
    }

    virtual void drawProgressBar(ElProgressBar *progressBar) = 0;
    virtual void drawButton(ElTextButton *button) = 0;
    virtual void drawWaveform(ElWaveform<uint16_t> *waveform) = 0;
    virtual void drawText(ElText *text) = 0;
};