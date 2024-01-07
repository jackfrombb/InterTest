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
            }
        }
    }

    virtual void drawButton(ElTextButton *button)
    {
        Serial.println("ERROR: WRONG USE OF VIRTUAL CLASS");
    }

    virtual void drawWaveform(ElWaveform<uint16_t> *waveform)
    {
        Serial.println("ERROR: WRONG USE OF VIRTUAL CLASS");
    }

    virtual void drawText(ElText *text)
    {
        Serial.println("ERROR: WRONG USE OF VIRTUAL CLASS");
    }
};