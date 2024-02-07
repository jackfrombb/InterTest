#pragma once
#include <U8g2lib.h>

// #include "interface_engine.h"
// #include "oscils/sync.h"

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

    // Буфер для заднего фона
    uint8_t *_displayBuffer;
    unsigned int bufferSize;

    typedef struct
    {
        uint16_t byteIndex;
        uint8_t bitIndex;
    } pixel_position;

    /// @brief Получить индекс пикселя в буфере для дисплея с вертикальными битами
    /// @param x позиция по х
    /// @param y позиция по у
    /// @param width ширина дисплея
    /// @return
    static pixel_position getPxPos(uint16_t x, uint16_t y, uint16_t width)
    {
        uint8_t bit_index = y & 7;
        uint16_t byte_index = ((y >> 3) * width) + x;
        return pixel_position{.byteIndex = byte_index, .bitIndex = bit_index};
    }

    // Эксперимент с буфером дисплея - не использовать
    //  Функция, которая затирает значения в буфере дисплея по координатам
    //  Параметры: buffer - указатель на буфер дисплея, width - ширина дисплея в пикселях, height - высота дисплея в пикселях
    //  x1, y1, x2, y2 - координаты прямоугольной области, которую нужно затереть
    //  fill - заполнить true
    static void voidArea(uint8_t *buffer, uint16_t width, uint16_t height,
                         uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, bool fill)
    {
        uint pixelCount = width * height;
        uint buffrLength = pixelCount >> 3;

        uint16_t start = (y1 * width) + x1;
        uint16_t end = (y2 * width) + x2;

        // Определяем индексы начального и конечного байтов в массиве
        uint16_t startByte = start >> 3;
        uint16_t endByte = end >> 3;

        // Определяем смещения начального и конечного битов в своих байтах
        uint8_t startBit = start & (8 - 1);
        uint8_t endBit = end & (8 - 1);

        // Инициализируем счетчик единичных битов
        uint count = 0;

        // Перебираем байты в массиве от начального до конечного
        for (int i = startByte; i <= endByte; i++)
        {
            // Создаем маску для выбора нужных битов в текущем байте
            byte mask = 0xFF; // 11111111 в двоичном виде

            // Если это начальный байт, то обнуляем старшие биты до начального бита
            if (i == startByte)
            {
                mask >>= startBit; // Сдвигаем маску вправо на startBit позиций
            }

            // Если это конечный байт, то обнуляем младшие биты после конечного бита
            if (i == endByte)
            {
                mask <<= (7 - endBit); // Сдвигаем маску влево на 7 - endBit позиций
                mask >>= (7 - endBit); // Сдвигаем маску обратно вправо на 7 - endBit позиций
            }

            // Применяем маску к текущему байту и считаем количество единичных битов в результате
            byte result = buffer[i] & mask; // Побитовое И между байтом и маской

            // Меняем значения битов в текущем байте на value
            if (fill == 0)
            {
                buffer[i] &= ~mask; // Побитовое И с инвертированной маской
            }
            else
            {
                buffer[i] |= mask; // Побитовое ИЛИ с маской
            }

            while (result > 0)
            {
                count += result & 1; // Прибавляем к счетчику младший бит результата
                result >>= 1;        // Сдвигаем результат вправо на одну позицию
            }
        }

        // Найти адреса байтов, стартовый и конечный, затереть значения бит
    }

    /// @brief Стереть область на экране (находящуюся за этим слоем)
    /// @param u8g2 указатель на библиотеку
    /// @param x1 горизонатльное положение левого верхнего угла
    /// @param y1 вертикальное положение левого верхненго угла
    /// @param x2 горизонтальное положение правого нижнего угла
    /// @param y2 вертикальное положение правого нижнего угла
    static void _eraseAreaOnDisplay(U8G2 *u8g2, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
    {
        // Затираем значения в буфере дисплея, устанавливая их в 0
        u8g2->setDrawColor(0);                   // Цвет рисования - черный
        u8g2->drawBox(x1, y1, x2 - x1, y2 - y1); // Рисуем прямоугольник по координатам
        u8g2->setDrawColor(1);                   // Возвращаем цвет рисования в белый

        u8g2->getBufferTileWidth();
    }

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

        // Это два вспомогательных пикселя, которые помогают оценить расположение
        //_u8g2->drawPixel(width - 1, height - 1);
        //_u8g2->drawPixel(0, 0);

        uint8_t widthPixelsCount = (float)width / waveform->getWidthSectionsCount();
        uint8_t heightPixelInSection = (float)height / waveform->getMaxMeasureValue();

        // Serial.println("Draw init OK");
        int voltSectionTitle = 0; // (int) waveform->getMaxMeasureValue();

        for (uint16_t v = height; v > 0; v -= heightPixelInSection)
        {
            for (uint16_t x = 0; x <= width; x += widthPixelsCount)
            {
                int titlePos = width - widthPixelsCount;

                if (x >= titlePos && voltSectionTitle != 0)
                {
                    String title = String(voltSectionTitle);
                    int xPos = titlePos;
                    int y = (int)(v + (_u8g2->getMaxCharHeight() * .5));

                    _u8g2->drawUTF8(xPos, y, title.c_str());
                }

                if (waveform->isNeedDrawBackDots())
                    _u8g2->drawPixel(x, v);
            }
            voltSectionTitle += 1;
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

        uint bias = measures.bias > measures.readedSize - width ? 0 : measures.bias; // SyncBuffer::findSignalOffset(waveform->getPoints(), waveform->getPointsLength());
        int vBias = 0;

        //  Преобразованный предел
        const int maxMeasureValNormalized = (int)(waveform->getMaxMeasureValue() * 1000);

        for (uint16_t x = bias; x < width + bias; x++)
        {
            uint32_t realVolt = measures.buffer[x]; //(int)_mainBoard->rawToVoltage(buf[x]);
            uint32_t next = x == width ? 0 : measures.buffer[x + 1];

            byte val = map(realVolt, 0, maxMeasureValNormalized, height - 1, 0);

            if (x == width + bias)
            {
                _u8g2->drawPixel(x - bias, val + vBias);
            }
            else
            {
                byte val2 = map(next, 0, maxMeasureValNormalized, height - 1, 0);
                _u8g2->drawLine(x - bias, val + vBias, (x - bias) + 1, val2 + vBias);
            }
        }

        // voidArea(_u8g2->getBufferPtr(), width, height, 10, 10, width - 10, height - 10, true);
    }

    const uint8_t *_getFontForSize(el_text_size size)
    {
        const uint8_t *ret = nullptr;
        switch (size)
        {
        case EL_VOLTMETER_VALUE_LARGE:
            ret = u8g2_font_12x6LED_mn;
            break;
        case EL_TEXT_SIZE_SUPER_LARGE:
            ret = u8g2_font_10x20_t_cyrillic;
            break;
        case EL_TEXT_SIZE_LARGE:
            ret = u8g2_font_8x13_t_cyrillic;
            break;
        case EL_TEXT_SIZE_MIDDLE:
            ret = u8g2_font_6x12_t_cyrillic;
            break;
        case EL_TEXT_SIZE_SMALL:
            ret = u8g2_font_5x7_t_cyrillic;
            break;
        case EL_TEXT_SIZE_SUPER_SMALL:
            ret = u8g2_font_4x6_t_cyrillic;
            break;
        }

        return ret;
    }

    static u8g2_uint_t u8g2_string_width(u8g2_t *u8g2, const char *str)
    {
        uint16_t e;
        u8g2_uint_t w, dx;
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
        int8_t initial_x_offset = -64;
#endif

        u8g2->font_decode.glyph_width = 0;
        u8x8_utf8_init(u8g2_GetU8x8(u8g2));

        /* reset the total width to zero, this will be expanded during calculation */
        w = 0;
        dx = 0;

        // printf("str=<%s>\n", str);

        for (;;)
        {
            e = u8g2->u8x8.next_cb(u8g2_GetU8x8(u8g2), (uint8_t)*str);
            if (e == 0x0ffff)
                break;
            str++;
            if (e != 0x0fffe)
            {

                dx = u8g2_GetGlyphWidth(u8g2, e); /* delta x value of the glyph */
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
                if (initial_x_offset == -64)
                    initial_x_offset = u8g2->glyph_x_offset;
#endif
                // printf("'%c' x=%d dx=%d w=%d io=%d ", e, u8g2->glyph_x_offset, dx, u8g2->font_decode.glyph_width, initial_x_offset);
                w += dx;
            }
        }
        // printf("\n");

        /* adjust the last glyph, check for issue #16: do not adjust if width is 0 */
        if (u8g2->font_decode.glyph_width != 0)
        {
            // printf("string width adjust dx=%d glyph_width=%d x-offset=%d\n", dx, u8g2->font_decode.glyph_width, u8g2->glyph_x_offset);
            w -= dx;
            w += u8g2->font_decode.glyph_width; /* the real pixel width of the glyph, sideeffect of GetGlyphWidth */
            /* issue #46: we have to add the x offset also */
            w += u8g2->glyph_x_offset; /* this value is set as a side effect of u8g2_GetGlyphWidth() */
#ifdef U8G2_BALANCED_STR_WIDTH_CALCULATION
            /* https://github.com/olikraus/u8g2/issues/1561 */
            if (initial_x_offset > 0)
                w += initial_x_offset;
#endif
        }
        // printf("w=%d \n", w);

        return w;
    }

    u8g2_uint_t _getUTF8Width(u8g2_t *u8g2, const char *str)
    {
        u8x8_char_cb charCb = u8x8_utf8_next;
        return u8g2_string_width(u8g2, str);
    }
    el_text_size currentFont;
    void _setTextSize(el_text_size size)
    {
        // if (currentFont == size)
        //     return;
        _u8g2->setFont(_getFontForSize(size));
        currentFont = size;
    }

    void _invertDisplayImg()
    {
        for (int i = 0; i < bufferSize; i++)
        {
            _u8g2->getBufferPtr()[i] = ~_u8g2->getBufferPtr()[i];
        }
    }

protected:
    void _onStartDraw() override
    {
        _u8g2->enableUTF8Print();
        _u8g2->firstPage();
    }

    void _onEndDraw() override
    {
        // if (inverImg)
        // {
        //     _invertDisplayImage();
        // }
        _u8g2->nextPage();
    }

    int _getTextCenterX(const String &text, int fromX, int width)
    {
        int textWidth = _u8g2->getUTF8Width(text.c_str());
        return (int)(fromX + ((float)width * 0.5) - ((float)textWidth * 0.5));
    }

private:
public:
    explicit InterfaceEngine_U8g2(DisplayVirtual *display)
    {
        _display = display;

        if (_display->getDisplayLibraryType() != display_library::DISPLAY_LIB_U8G2)
            throw "The library type is incorrect. It should be U8g2";

        _u8g2 = (U8G2 *)_display->getLibrary();

        // Выделяем место для хранения статичного буфера
        bufferSize = (unsigned int)(8 * _u8g2->getBufferTileHeight() * _u8g2->getBufferTileWidth());
        _displayBuffer = (uint8_t *)calloc(bufferSize, sizeof(uint8_t));
    }

    ~InterfaceEngine_U8g2()
    {
        free(_displayBuffer);
    }

    uint8_t getMaxTextWidth(el_text_size textSize) override
    {
        auto font = _getFontForSize(textSize);
        font += 9; // Число взято из метода https://github.com/olikraus/u8g2/blob/master/csrc/u8g2_font.c#L131
        return u8x8_pgm_read(font);
    }

    uint8_t getMaxTextHeight(el_text_size textSize) override
    {
        auto font = _getFontForSize(textSize);
        font += 10; // Число взято из метода https://github.com/olikraus/u8g2/blob/master/csrc/u8g2_font.c#L131
        return u8x8_pgm_read(font);
    }

    void drawCenteredGroup(ElCenteredGroup *group) override
    {
        if (group->isNeedDrawFrameAround())
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

        drawText(button);                                          // Выводим текст
        if (button->isSelected() && button->getEditPosition() < 0) // если активна то рисуем рамку вокруг
        {
            int16_t x = (button->getParent()->getX() + button->getX()) - 2;
            int16_t y = (button->getParent()->getY() + button->getY()) - 2;
            int16_t w = _u8g2->getUTF8Width(button->getText().c_str()) + 4;
            int16_t h = _u8g2->getMaxCharHeight() + 4;
            uint8_t r = 2;
            _u8g2->drawRFrame(x, y, w, h, r);
        }
    }

    void drawWaveform(ElWaveform *waveform) override
    {
        if (waveform->isNeedDrawBackground())
            _drawDotBack(waveform);
        _drawWaveform(waveform);
    }

    void drawText(ElText *text) override
    {
        _setTextSize(text->getTextSize()); // Размер и шрифт. Обязательно вызывать перед расчетом положения

        String textTitle = text->getText();

        int16_t x = text->getX();
        int y = text->getY();
        int w = _u8g2->getUTF8Width(textTitle.c_str());
        int h = _u8g2->getMaxCharHeight();

        if (text->getAlignment() == el_text_align::EL_TEXT_ALIGN_CENTER)
        {
            x += _getTextCenterX(textTitle, x + text->getParent()->getX(), text->getWidth());
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

        //_eraseAreaOnDisplay(_u8g2, textX, textY, _u8g2->getUTF8Width(textTitle.c_str()), _u8g2->getMaxCharHeight());
        // Отрисовать текст (y эллемеента делаем по верхнему углу)
        _u8g2->drawUTF8(textX, textY, textTitle.c_str());

        if (text->getEditPosition() >= 0 && textTitle.length() > 0)
        {
            uint8_t maxPosition = text->getText().length() - 1;                    // Кол-во символов
            uint8_t pos = std::min<uint8_t>(maxPosition, text->getEditPosition()); // Находим положение подчеркнутого символа
            String sub = textTitle.substring(0, pos);                              // Строка до подчеркнутого
            String subText = (String)textTitle[pos];                               // Сам символ
            uint8_t subWidth = _u8g2->getUTF8Width(subText.c_str());               // Ширина символа в пикс
            uint16_t textWidth = _u8g2->getUTF8Width(sub.c_str());                 // Расстояние от начала текста до символа

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

    void drawDisplayTest(ElDisplayTest *displayTest) override
    {
        // bool fill = true;
        static uint16_t prevIndex = 0;
        static uint8_t prevMask = 0;
        static uint8_t frameStop = 0;
        static uint16_t prevX = 0;
        static uint16_t prevY = 0;

        uint8_t *buffer = _u8g2->getBufferPtr();

        uint16_t width = displayTest->getWidth();
        uint16_t height = displayTest->getHeight();

        getPxPos(prevX, prevY, width);

        prevX += 1;
        if (prevX == width - 1)
        {
            prevX = 0;
            prevY = range(prevY + 1, 0, height - 1, true);
        }

        // int16_t x1 = 10, y1 = 10, x2 = width - 10, y2 = height - 10;

        // uint pixelCount = width * height;
        // uint buffrLength = pixelCount >> 3;

        // uint8_t byte = bitRead(buffer[prevIndex], prevMask);
        // BIT_TOGGLE(buffer[prevIndex], prevMask);

        // if (frameStop > 10)
        // {
        //     getPxPos(buffer, width >> 1, height >> 1, width, height);
        //     if (prevMask >= 7)
        //     {
        //         prevMask = 0;
        //         prevIndex = range(prevIndex + 1, 0, buffrLength, true);
        //     }
        //     else
        //     {
        //         prevMask += 1;
        //     }
        //     Serial.println("Index=" + String(prevIndex) + ";Mask=" + String(prevMask));
        //     frameStop = 0;
        // }
        // else
        //     frameStop += 1;
    }
};