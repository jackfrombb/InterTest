#pragma once
#include "interface_engine.h"
#include <U8g2lib.h>
#include "displays/display_virtual.h"
#include "board_virtual.h"
#include "oscils/sync.h"

// #define LCDWidth                        u8g2.getDisplayWidth()
// #define ALIGN_CENTER(t)                 ((LCDWidth - (u8g2.getUTF8Width(t))) / 2)
// #define ALIGN_RIGHT(t)                  (LCDWidth -  u8g2.getUTF8Width(t))
// #define ALIGN_LEFT                      0

class InterfaceEngine_U8g2 : public InterfaceEngineVirtual
{

private:
    U8G2 *_u8g2;
    DisplayVirtual *_display;
    MainBoard *_mainBoard;

    // Буфер для заднего фона
    uint8_t *_displayBuffer;
    unsigned int bufferSize;

    /// @brief Отрисовать ориентиры и надписи (буферезируется)
    void _drawDotBack(ElWaveform<uint16_t> *waveform)
    {
        static bool buffered = false; // Флаг буферизации фона

        if (buffered)
        {
            uint8_t *buf = _u8g2->getBufferPtr();
            // Размер буфера равен 8 * u8g2.getBufferTileHeight () * u8g2.getBufferTileWidth ().
            memcpy(buf, _displayBuffer, bufferSize);
            return;
        }

        _u8g2->setFont(u8g2_font_4x6_tr);

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();

        uint8_t heightPixelInSection = height / waveform->getHeightSectionsCount();
        uint8_t widthPixelsCount = width / waveform->getWidthSectionsCount();

        // Serial.println("Draw init OK");
        int voltSectionTitle = (int)waveform->getMaxMeasureValue() - 1;

        for (uint16_t v = 0; v <= height; v += heightPixelInSection)
        {
            for (uint16_t x = 0; x <= width; x += widthPixelsCount)
            {
                int titlePos = width - widthPixelsCount;
                if (x >= titlePos)
                {
                    String title = String(voltSectionTitle);
                    int xPos = titlePos;
                    int y = (int)(v + (_u8g2->getMaxCharHeight() * 1.5));

                    _u8g2->drawUTF8(xPos, y, title.c_str());

                    voltSectionTitle -= 1;
                }

                _u8g2->drawPixel(x, v);
            }
        }

        memcpy(_displayBuffer, _u8g2->getBufferPtr(), bufferSize);
        buffered = true;
    }

    /// @brief Отрисовать график
    /// @param waveform данные осциллограммы
    void _drawWaveform(ElWaveform<uint16_t> *waveform)
    {
        // Если происходит буфер уже занят, то задерживаем поток и ждем
        // иначе занимаем буфер
        if (waveform->getOscil()->isBufferBussy())
        {
            delayMicroseconds(100);
            _drawWaveform(waveform);
            return;
        }
        else
        {
            waveform->getOscil()->setBufferBussy(true);
        }

        int bias = 0; // SyncBuffer::findSignalOffset(waveform->getPoints(), waveform->getPointsLength());

        //  Преобразованный предел
        const int maxMeasureValNormalized = (int)(waveform->getMaxMeasureValue() * 1000);

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();
        auto *buf = waveform->getPoints();

        int max = 0;

        for (uint16_t x = bias; x <= width + bias; x++)
        {
            int realVolt = (int)_mainBoard->rawToVoltage(buf[x]);
            // logi::p("Engine", "Raw: " + String(buf[x]) + " Volt: "+ String(realVolt));

            int next = x == width ? 0 : (int)buf[x + 1];
            if (realVolt > max)
                max = realVolt;
            // logi::p("Engine", "RealV: " + String(realVolt) + " Max: " + String(max));

            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width + bias)
            {
                _u8g2->drawPixel(x - bias, val);
            }
            else
            {
                byte val2 = map((long)_mainBoard->rawToVoltage(next), 0, maxMeasureValNormalized, height - 1, 0);
                _u8g2->drawLine(x - bias, val, (x - bias) + 1, val2);
            }
        }
        waveform->getOscil()->setBufferBussy(false);
    }

    void _setTextSize(el_text_size size)
    {
        static el_text_size currentFont;

        // if (currentFont == size)
        //     return;

        switch (size)
        {
        case EL_TEXT_SIZE_SUPER_LARGE:
            _u8g2->setFont(u8g2_font_10x20_t_cyrillic);
            break;
        case EL_TEXT_SIZE_LARGE:
            _u8g2->setFont(u8g2_font_8x13_t_cyrillic);
            break;
        case EL_TEXT_SIZE_MIDDLE:
            _u8g2->setFont(u8g2_font_6x12_t_cyrillic);
            break;
        case EL_TEXT_SIZE_SMALL:
            _u8g2->setFont(u8g2_font_5x7_t_cyrillic);
            break;
        case EL_TEXT_SIZE_SUPER_SMALL:
            _u8g2->setFont(u8g2_font_4x6_t_cyrillic);
            break;
        }

        currentFont = size;
    }

protected:
    void _onStartDraw() override
    {
        _u8g2->enableUTF8Print();
        _u8g2->firstPage();
    }

    void _onEndDraw() override
    {
        _u8g2->nextPage();
    }

    int _getTextCenterX(const String &text, int fromX, int width)
    {
        int textWidth = _u8g2->getUTF8Width(text.c_str());
        return (int)(fromX + ((float)width * 0.5) - ((float)textWidth * 0.5));
    }

private:
public:
    explicit InterfaceEngine_U8g2(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;

        _display = mainBoard->getDisplay();
        _u8g2 = (U8G2 *)_display->getLibrary();
        bufferSize = (unsigned int)(8 * _u8g2->getBufferTileHeight() * _u8g2->getBufferTileWidth());
        _displayBuffer = (uint8_t *)calloc(bufferSize, sizeof(uint8_t));
    }

    ~InterfaceEngine_U8g2()
    {
        free(_displayBuffer);
    }

    void drawCenteredGroup(ElCenteredGroup *group) override
    {
        _u8g2->drawRFrame(group->getX(), group->getY(), group->getWidth(), group->getHeight(), 2);

        if (group->getElementsCount() == 0)
        {
            return;
        }

        auto newWidth = group->getWidth() / group->getElementsCount();
        auto groupArea = group->getArea();

        int16_t prevX = 0;

        for (int i = 0; i < group->getElementsCount(); i++)
        {
            ElementVirtual *el = group->getElement(i);
            el->setWidth(newWidth);
            el->setX(newWidth * i);

            drawElement(el);
        }
    }

    void drawButton(ElTextButton *button) override
    {
        drawText(button);

        int x = _getTextCenterX(button->getText(), button->getX(), button->getWidth()) + button->getParent()->getX();
        int y = (int)(button->getParent()->getY() + (button->getParent()->getHeight() * 0.5) - ((_u8g2->getMaxCharHeight() + 2) * 0.5));

        if (button->isPushed()) // если нажата то заполненный скругленый прямоугольник
        {
            _u8g2->drawRBox(x - 4,
                            y,
                            _u8g2->getUTF8Width(button->getText().c_str()) + 8,
                            _u8g2->getMaxCharHeight() + 2,
                            2);
        }
        else if (button->isSelected()) // если активна то рисуем рамку вокруг
        {
            _u8g2->drawRFrame(x - 4,
                              y,
                              _u8g2->getUTF8Width(button->getText().c_str()) + 8,
                              _u8g2->getMaxCharHeight() + 2,
                              2);
        }
    }

    void drawWaveform(ElWaveform<uint16_t> *waveform) override
    {
        _drawDotBack(waveform);
        _drawWaveform(waveform);
    }

    void drawText(ElText *text) override
    {
        _setTextSize(text->getTextSize()); // Размер и шрифт. Обязательно вызывать перед расчетом положения

        int x = text->getX();
        int y = text->getY();

        if (text->getAlignment() == el_text_align::EL_TEXT_ALIGN_CENTER)
        {
            x = _getTextCenterX(text->getText(), x, text->getWidth());
        }
        else if (text->getAlignment() == el_text_align::EL_TEXT_ALIGN_RIGHT)
        {
            x = text->getWidth() - _u8g2->getUTF8Width(text->getText().c_str());
        }

        if (text->getVerticalAlignment() == el_vertical_align::EL_ALIGN_CENTER)
        {
            y = (int)((text->getParent()->getHeight() * 0.5) - (_u8g2->getMaxCharHeight() * 0.5));
        }

        // Отрисовать текст (y эллемеента делаем по верхнему углу)
        _u8g2->drawUTF8(x + text->getParent()->getX(),
                        y + _u8g2->getMaxCharHeight() + text->getParent()->getY(),
                        text->getText().c_str());
    }

    void drawProgressBar(ElProgressBar *progressBar) override
    {
        // Serial.println("Progress: " + String(progressBar->getProgress()));

        _u8g2->drawRFrame(progressBar->getX(), progressBar->getY(), progressBar->getWidth(), progressBar->getHeight(), 2);

        int progressLineWidth = (int)((float)(progressBar->getWidth() - 4) * progressBar->getProgress());

        progressLineWidth = max(progressLineWidth, 0);
        progressLineWidth = min(progressLineWidth, progressBar->getWidth() - 4);
        _u8g2->drawBox(progressBar->getX() + 2, progressBar->getY() + 2, progressLineWidth, (progressBar->getHeight() - 4));
    }
};