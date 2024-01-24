#pragma once
#include "interface_engine.h"
#include <U8g2lib.h>
#include "displays/display_virtual.h"
#include "board_virtual.h"
#include "oscils/sync.h"

#define PAGE_TAG "Engine_U8G2"

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
    void _drawDotBack(ElWaveform *waveform)
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

                if (x >= titlePos && voltSectionTitle > 0)
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
    void _drawWaveform(ElWaveform *waveform)
    {
        auto measures = waveform->getMeasures();

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();

        int bias = measures.bias > measures.bufferSize - (width + 1) ? 0 : measures.bias - 1; // SyncBuffer::findSignalOffset(waveform->getPoints(), waveform->getPointsLength());
        bias = max(bias, 0);
        //  Преобразованный предел
        const int maxMeasureValNormalized = (int)(waveform->getMaxMeasureValue() * 1000);

        for (uint16_t x = bias; x <= width + bias; x++)
        {
            uint32_t realVolt = measures.buffer[x]; //(int)_mainBoard->rawToVoltage(buf[x]);
            uint32_t next = x == width ? 0 : measures.buffer[x + 1];

            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width + bias)
            {
                _u8g2->drawPixel(x - bias, val);
            }
            else
            {
                byte val2 = map(next, 0, maxMeasureValNormalized, height - 1, 0);
                _u8g2->drawLine(x - bias, val, (x - bias) + 1, val2);
            }
        }
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

        if (_display->getDisplayLibraryType() != display_library::DISPLAY_LIB_U8G2)
            throw "The library type is incorrect. It should be U8g2";

        _u8g2 = (U8G2 *)_display->getLibrary();

        //Выделяем место для хранения статичного буфера
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

    void drawLine(ElLine *line) override
    {
        _u8g2->drawLine(line->getX(), line->getY(), line->getArea().rightDown.x, line->getArea().rightDown.y);
    }

    void drawButton(ElTextButton *button) override
    {
        drawText(button);

        int x = button->getParent()->getX() + button->getX();
        int y = button->getParent()->getY() + button->getY();

        if (button->isSelected() && button->getEditPosition() < 0) // если активна то рисуем рамку вокруг
        {
            _u8g2->drawRFrame(x - 2,
                              y - 2,
                              _u8g2->getUTF8Width(button->getText().c_str()) + 4,
                              _u8g2->getMaxCharHeight() + 4,
                              2);
        }
    }

    void drawWaveform(ElWaveform *waveform) override
    {
        _drawDotBack(waveform);
        _drawWaveform(waveform);
    }

    void drawText(ElText *text) override
    {
        _setTextSize(text->getTextSize()); // Размер и шрифт. Обязательно вызывать перед расчетом положения

        String textTitle = text->getText();

        int x = text->getX();
        int y = text->getY();

        if (text->getAlignment() == el_text_align::EL_TEXT_ALIGN_CENTER)
        {
            x = _getTextCenterX(textTitle, x, text->getWidth());
        }
        else if (text->getAlignment() == el_text_align::EL_TEXT_ALIGN_RIGHT)
        {
            x = text->getWidth() - _u8g2->getUTF8Width(textTitle.c_str());
        }

        if (text->getVerticalAlignment() == el_vertical_align::EL_ALIGN_CENTER)
        {
            y = (int)((text->getParent()->getHeight() * 0.5) - (_u8g2->getMaxCharHeight() * 0.5));
        }

        uint16_t textX = x + text->getParent()->getX();
        uint16_t textY = y + _u8g2->getMaxCharHeight() + text->getParent()->getY();

        // Отрисовать текст (y эллемеента делаем по верхнему углу)
        _u8g2->drawUTF8(textX, textY, textTitle.c_str());

        if (text->getEditPosition() >= 0 && textTitle.length() > 0)
        {
            uint8_t maxPosition = text->getText().length() - 1;
            uint8_t pos = std::min<uint8_t>(maxPosition, text->getEditPosition());
            String sub = textTitle.substring(0, pos);
            String subText = (String)textTitle[pos];
            uint8_t subWidth = _u8g2->getUTF8Width(subText.c_str());
            uint16_t textWidth = _u8g2->getUTF8Width(sub.c_str());

            _u8g2->drawLine((textX + textWidth) - (subWidth + 2), textY + 2, textX + textWidth - 1, textY + 2);
        }
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