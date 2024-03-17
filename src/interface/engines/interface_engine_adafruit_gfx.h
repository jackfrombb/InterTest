#pragma once

// TODO: удалить ссылки на библиотеки после включения класса в проект
#include "interface_engine.h"
#include "interface/ellements/ellements_list.h"
#include "displays/display_virtual.h"

#define TEXT_PADDING 10

// https://github.com/Bodmer/U8g2_for_TFT_eSPI - библиотека шрифтов u8g2

class InterfaceEngine_ArduinoGfx : public InterfaceEngineVirtual
{
private:
    DisplayVirtual *_display;
    TFT_eSPI *_tft;
    TFT_eSprite *_spr;

    /// @brief Отрисовать ориентиры и надписи (буферезируется)
    void _drawDotBack(ElWaveform *waveform, uint16_t maxMeasure)
    {
        if (!waveform->isNeedDrawBackground())
        {
            return;
        }

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();

        uint8_t widthPixelsCount = (float)width / waveform->getWidthSectionsCount();
        uint8_t heightPixelInSection = (float)height / waveform->getHeightSectionsCount();

        float voltSectionTitle = (float)maxMeasure / 1000 - 1; // (int) waveform->getMaxMeasureValue();
        const float voltsInSection = ((float)maxMeasure / 1000 - 1) / waveform->getHeightSectionsCount();

        for (uint16_t v = 0; v <= height; v += heightPixelInSection)
        {
            for (uint16_t x = 0; x <= width; x += widthPixelsCount)
            {
                int titlePos = width - widthPixelsCount;

                if (x >= titlePos)
                {
                    String title = String(voltSectionTitle);
                    int xPos = titlePos;
                    int y = (int)(v + (_spr->fontHeight() * .5));

                    _spr->drawString(title, xPos, y);
                }

                if (waveform->isNeedDrawBackDots())
                    _spr->drawPixel(x, v, TFT_SILVER);
            }
            voltSectionTitle -= voltsInSection;
        }
    }

    /// @brief Отрисовать график
    /// @param waveform данные осциллограммы
    void _drawWaveform(ElWaveform *waveform)
    {
        auto measures = waveform->getMeasures();

        //  Преобразованный предел
        const int maxMeasureValNormalized = max(4000, measures.max + 1); //(int)(waveform->getMaxMeasureValue() * 1000);
        _drawDotBack(waveform, maxMeasureValNormalized);

        uint16_t width = waveform->getArea().getWidth();
        uint16_t height = waveform->getArea().getHeight();

        uint bias = measures.bias > measures.readedSize - width ? 0 : measures.bias; // SyncBuffer::findSignalOffset(waveform->getPoints(), waveform->getPointsLength());
        int vBias = 0;

        for (uint16_t x = bias; x < width + bias; x++)
        {
            uint32_t realVolt = measures.buffer[x]; //(int)_mainBoard->rawToVoltage(buf[x]);
            uint32_t next = x == width ? 0 : measures.buffer[x + 1];

            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width + bias)
            {
                _spr->drawPixel(x - bias, val + vBias, TFT_WHITE);
            }
            else
            {
                byte val2 = map(next, 0, maxMeasureValNormalized, height - 1, 0);
                _spr->drawLine(x - bias, val + vBias, (x - bias) + 1, val2 + vBias, TFT_WHITE);
            }
        }

        // voidArea(_u8g2->getBufferPtr(), width, height, 10, 10, width - 10, height - 10, true);
    }

    void setFornt(el_text_size size)
    {
        switch (size)
        {
        case EL_VOLTMETER_VALUE_LARGE:
            _spr->setTextSize(5);
            break;
        case EL_TEXT_SIZE_SUPER_LARGE:
            _spr->setTextSize(4);
            break;
        case EL_TEXT_SIZE_LARGE:
            _spr->setTextSize(2);
            break;
        case EL_TEXT_SIZE_MIDDLE:
            _spr->setTextSize(1);
            break;
        case EL_TEXT_SIZE_SMALL:
            _spr->setTextSize(1);
            break;
        case EL_TEXT_SIZE_SUPER_SMALL:
            _spr->setTextSize(1);
            break;
        }
    }

    /// @brief Получить цвет RGB(5:6:5)
    /// @param red значение красного
    /// @param green значение зеленого
    /// @param blue значение синего
    uint16_t convertRGBto565(uint8_t red, uint8_t green, uint8_t blue)
    {
        red = red * 31 / 255;     // масштабирование красного цвета (0-31)
        green = green * 63 / 255; // масштабирование зеленого цвета (0-63)
        blue = blue * 31 / 255;   // масштабирование синего цвета (0-31)

        return (red << 11) | (green << 5) | blue;
    }

public:
    explicit InterfaceEngine_ArduinoGfx(DisplayVirtual *display)
    {
        _display = display;

        if (_display->getDisplayLibraryType() != display_library::DISPLAY_LIB_ARDUINO_GFX)
            throw "The library type is incorrect. It should be AdafruitGFX";

        _tft = (TFT_eSPI *)_display->getLibrary();

        _spr = new TFT_eSprite(_tft);                      // Создаем спрайт
        _spr->createSprite(_tft->width(), _tft->height()); // Создаем спрайт размером 128x160 пикселей
    }

    ~InterfaceEngine_ArduinoGfx() override
    {
        delete _spr;
        _spr = nullptr;
    }

    void _onStartDraw() override
    {
        _spr->fillSprite(TFT_BLACK);
    }

    void _onEndDraw() override
    {
        _spr->pushSprite(0, 0);
    }

    void drawProgressBar(ElProgressBar *progressBar) override
    {
        _spr->fillRoundRect(progressBar->getX(), progressBar->getY(),
                            progressBar->getWidth(), progressBar->getHeight(),
                            2, TFT_WHITE);

        int progressLineWidth = (int)((float)(progressBar->getWidth() - 4) * progressBar->getProgress());

        progressLineWidth = max(progressLineWidth, 0);
        progressLineWidth = min(progressLineWidth, (int)(progressBar->getWidth() - 4));
        _spr->fillRect(progressBar->getX() + 2, progressBar->getY() + 2, progressLineWidth, (progressBar->getHeight() - 4), _spr->color565(35, 0, 27));
    }

    display_position drawButton(ElTextButton *button) override
    {
        display_position pos = drawText(button); // Выводим текст

        if (button->isSelected() && button->getEditPosition() < 0) // если активна то рисуем рамку вокруг
        {
            uint8_t padding = 3;
            int16_t x = pos.getX();
            int16_t y = pos.getY();
            int16_t w = pos.getWidth();
            int16_t h = pos.getHeight();
            uint8_t r = 2;
            // Рисуем рамку с отступами, а по Y вычитаем высоту строки, иначе рамка будет сдвинута вниз
            _spr->drawRoundRect(x - padding, y - padding, w + padding * 2, h + padding * 2, r, TFT_WHITE);
        }

        return pos;
    }

    void drawWaveform(ElWaveform *waveform) override
    {
        _drawWaveform(waveform);
    }

    display_position drawText(ElText *text) override
    {
        setFornt(text->getTextSize());
        //_spr->setTextSize(1);

        if (text->isWidthMatchParent())
        {
            text->setWidth((uint32_t)text->getParent()->getWidth());
        }

        if (text->isHeightMatchParent())
        {
            text->setHeight((uint32_t)text->getParent()->getHeight());
        }

        String title = text->getText();
        int x = text->getX();
        int y = text->getY();
        int w = _spr->textWidth(title);
        int h = _spr->fontHeight();

        // Serial.println("Text: " + title + " X: " + String(x) + " Y: " + String(y) + " W: " + String(w) + " H: " + String(h));

        switch (text->getAlignment())
        {
        case el_text_align::EL_TEXT_ALIGN_CENTER_PARENT:
            x += (text->getParent()->getWidth() >> 1) - (w >> 1);
            break;

        case el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH:
            x += (text->getWidth() >> 1) - (w >> 1);
            break;

        case el_text_align::EL_TEXT_ALIGN_RIGHT:
            x += text->getWidth() - w;
            break;

        case el_text_align::EL_TEXT_ALIGN_LEFT:
        default:
            break;
        }

        if (text->getVerticalAlignment() == el_vertical_align::EL_ALIGN_CENTER)
        {
            y += (text->getParent()->getHeight() >> 1) - (h >> 1);
        }
        else if (text->getVerticalAlignment() == el_vertical_align::EL_TEXT_ALIGN_SELF_CENTER)
        {
            y += (text->getHeight() >> 1) - (h >> 1);
        }

        // Отрисовать текст опираясь на стиль заголовка
        if (text->getVisualStyleFlags() & STYLE_MAIN_TITLE)
        {
            _spr->fillRoundRect(text->getX() + 5, text->getY() + 2, text->getWidth() - 10, text->getHeight() - 2, 2, TFT_SILVER);
        }
        _spr->setTextColor(TFT_WHITE); // Устанавливаем цвет текста на белый

        _spr->drawString(title, x, y);

        // Если в режиме посимвольного редактирования то рисуем линию под символом, который редактируется
        if (text->isInEditMode())
        {
            // Отсчет идет от 1, потому длина строки и есть последний символ
            uint8_t maxPosition = text->getText().length();                        // Кол-во символов
            uint8_t pos = std::min<uint8_t>(maxPosition, text->getEditPosition()); // Находим положение подчеркнутого символа
            String sub = title.substring(0, pos - 1);                              // Строка до подчеркнутого
            String subText = (String)title[pos - 1];                               // Сам символ
            uint8_t subWidth = _spr->textWidth(subText);                           // Ширина символа в пикс
            uint16_t textWidth = _spr->textWidth(sub.c_str());                     // Расстояние от начала текста до символа

            _spr->drawLine(x + textWidth - 1, y + 4 + (h >> 1), x + textWidth + subWidth - 1, y + 4 + (h >> 1), TFT_RED);
            _spr->drawLine(x + textWidth - 1, y + 5 + (h >> 1), x + textWidth + subWidth - 1, y + 5 + (h >> 1), TFT_RED);
            // Serial.println("Sub: " + sub);
        }

        return display_position{{.x = x, .y = y}, {.x = x + w, .y = y + h}};
    }

    void drawCenteredGroup(ElCenteredGroup *group) override
    {
        if (group->isNeedDrawFrameAround())
            _spr->drawRoundRect(group->getX(), group->getY(), group->getWidth(), group->getHeight(), 2, TFT_WHITE);

        InterfaceEngineVirtual::drawCenteredGroup(group);
    }

    void drawLine(ElLine *line) override
    {
        auto x = line->getX();
        auto y = line->getY();
        auto x1 = line->getArea().rightDown.x;
        auto y1 = line->getArea().rightDown.y;

        _spr->drawLine(x, y, x1, y1, TFT_WHITE);
    }

    void drawDisplayTest(ElDisplayTest *displayTest)
    {
        // _tft->drawPixel(10, 10, TFT_RED);
    }

    // TODO: Сделать индикатор зарядки
    void drawBatteryIndicr(ElBattery *batteryIndcr) override
    {
    }

    void drawScrollbar(ElScrollBar *scrollbar) override
    {
        // Разные, но похожие способы рисования для вертикальных и горизонтальный полос прокрутки
        // надо будет оптимизировать код
        if (scrollbar->isVertical())
        {
            int scrollHeight = (int)((float)(scrollbar->getHeight()) * scrollbar->getDisplayedWidth());
            int scrollY = scrollbar->getY() + (scrollbar->getHeight() * scrollbar->getScrollPosition()) + 1;

            _spr->fillRoundRect(scrollbar->getX() - 2, scrollbar->getY() - 2,
                                scrollbar->getWidth() + 2, scrollbar->getHeight() + 2,
                                2, _spr->color565(35, 0, 27));

            _spr->fillRoundRect(scrollbar->getX(),
                                scrollY,
                                scrollbar->getWidth(),
                                scrollHeight,
                                1, TFT_WHITE);

            // Точки для отслеживания высоты
            // _spr->drawPixel(scrollbar->getX() + (scrollbar->getWidth() >> 1), scrollbar->getY(), TFT_SILVER);
            // _spr->drawPixel(scrollbar->getX() + (scrollbar->getWidth() >> 1), scrollbar->getY() + scrollbar->getHeight(), TFT_SILVER);
        }
        else
        {
            int scrollWidth = (int)((float)(scrollbar->getWidth() - 4) * scrollbar->getDisplayedWidth());
            int scrollX = scrollbar->getX() + (scrollbar->getWidth() * scrollbar->getScrollPosition()) + 1;

            _spr->fillRoundRect(scrollbar->getX() - 2, scrollbar->getY() - 2,
                                scrollbar->getWidth() + 2, scrollbar->getHeight() + 2,
                                2, _spr->color565(35, 0, 27));

            _spr->fillRoundRect(scrollX,
                                scrollbar->getY(),
                                scrollWidth,
                                scrollbar->getHeight(),
                                1, TFT_WHITE);
            // Точки для отслеживания ширины
            // _u8g2->drawPixel(scrollbar->getX(), scrollbar->getY() + (scrollbar->getHeight() >> 1));
            // _u8g2->drawPixel(scrollbar->getX() + scrollbar->getWidth(), scrollbar->getY() + (scrollbar->getHeight() >> 1));
        }
    }

    uint8_t getMaxTextWidth(el_text_size textSize) override
    {
        return _spr->textWidth("A");
    }

    uint8_t getMaxTextHeight(el_text_size textSize) override
    {
        return _spr->fontHeight();
    }
};
