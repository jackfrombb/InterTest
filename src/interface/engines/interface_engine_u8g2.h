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
                drawButton((ElButton *)page.getEllement(i));
                break;

            case EL_TYPE_WAVEFORM:
                drawWaveform((ElWaveform<int16_t>*)page.getEllement(i));
                break;

            case EL_TYPE_TEXT:
                drawText((ElText*) page.getEllement(i));
                break;
            }
        }
    }

    virtual void drawButton(ElButton *button)
    {

    }

    virtual void drawWaveform(ElWaveform<int16_t> *waveform)
    {

    }

    virtual void drawText(ElText *text)
    {

    }
};