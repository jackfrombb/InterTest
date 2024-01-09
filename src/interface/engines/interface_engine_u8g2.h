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
            int realVolt = _mainBoard->rawToVoltage(buf[x]);
            int next = x == width ? 0 : buf[x + 1];

            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width - 1)
            {
                _u8g2->drawPixel(x, val);
            }
            else
            {
                byte val2 = map(_mainBoard->rawToVoltage(next), 0, maxMeasureValNormalized, height - 1, 0);
                _u8g2->drawLine(x, val, x + 1, val2);
            }
        }
    }

    void _setTextSize(el_text_size size)
    {
        switch (size)
        {
        case EL_SIZE_SUPER_LARGE:
            _u8g2->setFont(u8g2_font_10x20_t_cyrillic);
            break;
        case EL_SIZE_LARGE:
            _u8g2->setFont(u8g2_font_8x13_t_cyrillic);
            break;
        case EL_SIZE_MIDDLE:
            _u8g2->setFont(u8g2_font_6x12_t_cyrillic);
            break;
        case EL_SIZE_SMALL:
            _u8g2->setFont(u8g2_font_5x7_t_cyrillic);
            break;
        case EL_SIZE_SUPER_SMALL:
            _u8g2->setFont(u8g2_font_4x6_t_cyrillic);
            break;

        default:
            break;
        }
    }

protected:
    void _onStartDraw()
    {
        _u8g2->firstPage();
    }

    void _onEndDraw()
    {
        _u8g2->nextPage();
    }

public:
    InterfaceEngine_U8g2(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
        _display = mainBoard->getDisplay();
        _u8g2 = (U8G2 *)_display->getLibrarry();
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
        _u8g2->drawStr(centered.x, centered.y, button->getTitle()->c_str());
    }

    virtual void drawWaveform(ElWaveform<uint16_t> *waveform)
    {
        _drawWaveform(waveform);
    }

    virtual void drawText(ElText *text)
    {
        _setTextSize(text->getEllementSize()); //Размер и шрифт. Обязательно вызывать перед расчетом положения

        int x = text->getX();
        int y = text->getY() + _u8g2->getMaxCharHeight();

        if (x == ELLEMENT_POSITION_CENTER)
        {
            uint8_t textLength = text->getText().length();
            int displayWidth =  _display->getResoluton().width;
            uint8_t maxCharWidth = _u8g2->getMaxCharWidth();
            x = (((float)displayWidth * 0.5) - ((float)(textLength *0.5) * (maxCharWidth)));
        }

        if (y == ELLEMENT_POSITION_CENTER)
        {
            y = (_display->getResoluton().height * 0.5) - (_u8g2->getMaxCharHeight() * 0.5);
        }

        // Отрисовать текст
        // _u8g2->setCursor(x, y);
        // _u8g2->print(text->getText());
        _u8g2->drawStr(x, y, text->getText().c_str());
    }

    virtual void drawProgressBar(ElProgressBar *progressBar)
    {
        // Serial.println("Progress: " + String(progressBar->getProgress()));

        _u8g2->drawRFrame(progressBar->getX(), progressBar->getY(), progressBar->getWidth(), progressBar->getHeight(), 2);

        int progressLineWidth = ((progressBar->getWidth() - 4) * progressBar->getProgress());

        progressLineWidth = max(progressLineWidth, 0);
        progressLineWidth = min(progressLineWidth, progressBar->getWidth() - 4);
        _u8g2->drawBox(progressBar->getX() + 2, progressBar->getY() + 2, progressLineWidth, (progressBar->getHeight() - 4));
    }
};