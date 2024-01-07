#pragma once
#include "interface_engine.h"
#include <U8g2lib.h>
#include "displays/display_virtual.h"
#include "board_virtual.h"

class InterfaceEngine_U8g2 : public InterfaceEngineVirtual
{
private:
    U8G2 *_u8g2;
    DisplayVirtual *_display;
    MainBoard *_mainBoard;

    /// @brief Отрисовать ориентиры и надписи
    void _drawDotBack(ElWaveform<uint16_t> *waveform)
    {
        _u8g2->setFont(u8g2_font_4x6_tr);

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();

        uint8_t heightPixelInSection = height / waveform->getHeightSectionsCount();
        uint8_t widthPixelsCount = width / waveform->getWidthSectionsCount();

        for (int8_t v = 1; v <= waveform->getHeightSectionsCount(); v += heightPixelInSection)
        {
            for (uint8_t x = 0; x < width - 1; x += widthPixelsCount)
            {
                int titlePos = width - widthPixelsCount;
                if (x >= titlePos)
                {
                    _u8g2->setCursor(titlePos, v + 10);
                    _u8g2->print(v);
                }
                _u8g2->drawPixel(x, v);
            }
        }
    }

    /// @brief Отрисовать график
    /// @param waveform данные осциллограммы
    void _drawWaveform(ElWaveform<uint16_t> *waveform)
    {
        int bias = 0;

        // Преобразованный предел
        const int maxMeasureValNormalized = _mainBoard->getMaxAdcMeasureValue() * 1000;

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();
        uint32_t *buf = (uint32_t *)waveform->getPoints();

        for (uint8_t x = 0; x <= width; x++)
        {
            int realVolt = esp_adc_cal_raw_to_voltage(*buf, _mainBoard->getAdcChars());
            int next = x == width ? 0 : buf[x + 1];

            // // Высчитывание среднего значения
            // mV = midArifm2(realVolt / 1000, displayWidth); // expRunningAverage(realVolt / 1000);
            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width - 1)
            {
                _u8g2->drawPixel(x, val);
            }
            else
            {
                byte val2 = map(esp_adc_cal_raw_to_voltage(next, _mainBoard->getAdcChars()), 0, maxMeasureValNormalized, height - 1, 0);
                _u8g2->drawLine(x, val, x + 1, val2);
            }
        }
    }

public:
    InterfaceEngine_U8g2(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
        _display = _mainBoard->getDisplay();
        _u8g2 = (U8G2 *)_display->getLibrarry();
    }

    virtual void drawButton(ElTextButton *button)
    {
    }

    virtual void drawWaveform(ElWaveform<uint16_t> *waveform)
    {

        _u8g2->firstPage();
        //_drawDotBack(waveform);
        _drawWaveform(waveform);
        _u8g2->nextPage();
    }

    virtual void drawText(ElText *text)
    {
    }
};