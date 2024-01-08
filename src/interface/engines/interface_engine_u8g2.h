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
        _u8g2 = (U8G2 *) _display->getLibrary();
    }

    virtual void drawButton(ElTextButton *button)
    {
        if (!button->isVisible())
            return;

        _u8g2->setFont(u8g2_font_5x8_t_cyrillic);
        int minHeight = _u8g2->getMaxCharHeight() + 6;
        int height = button->getHeight() < minHeight ? minHeight : button->getHeight();

        if (button->isPushed()) // если нажата то заполненный скругленый прямоугольник
        {
            _u8g2->drawRBox(button->getX(), button->getY(), button->getWidth(), height, 4);
        }
        else if (button->isSelected()) // если активна то рисуем рамку вокруг
        {
            _u8g2->drawRFrame(button->getX(), button->getY(), button->getWidth(), height, 4);
        }
        // Иначе просто текст

        // Надпись по центру кнопки
        point_t centered = getTextCenter(button->getTitle()->length(), button->getArea(),
                                     _u8g2->getMaxCharWidth(), _u8g2->getMaxCharHeight());
        // Отрисовать текст
        _u8g2->drawStr(centered.x, centered.y, (const char *)button->getTitle());
    }

    virtual void drawWaveform(ElWaveform<uint16_t> *waveform)
    {
        //_drawDotBack(waveform);
        _drawWaveform(waveform);
        _u8g2->nextPage();
    }

    virtual void drawText(ElText *text)
    {
        int x = text->getX();
        int y = text->getY();

        if(x == ELLEMENT_POSITION_CENTER) {
            x = getTextCenterX(text->getText()->length(), 0, display->getResolution().width, _u8g2->getMaxCharWidth());

        }

        if(y == ELLEMENT_POSITION_CENTER) {
            y = (_display->getResolution().height * 0.5) - (_u8g2->getMaxCharHeight() * 0.5);
        }

        // Отрисовать текст
        _u8g2->drawStr(text->getX(), text->getY(), (const char *)text->getText());
    }

    virtual void drawProgressBar(ElProgressBar *progressBar)
    {
        _u8g2->drawFrame(progressBar->getX(), progressBar->getY(), progressBar->getWidth(), progressBar->getHeight());
        _u8g2->drawBox(progressBar->getX() + 2, progressBar->getY() + 2,
                       (progressBar->getWidth() - 4) * progressBar->getProgress(), (progressBar->getHeight() - 3));
    }
};