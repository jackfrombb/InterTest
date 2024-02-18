#pragma once

// TODO: удалить ссылки на библиотеки после включения класса в проект
#include "interface_engine.h"
#include "interface/ellements/ellements_list.h"
#include "displays/display_virtual.h"

class InterfaceEngine_ArduinoGfx : public InterfaceEngineVirtual
{
private:
    DisplayVirtual *_display;
    TFT_eSPI *_tft;
    TFT_eSprite *_spr;

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
        // _tft->fillScreen(TFT_BLACK);
        // _tft->drawPixel(10, 10, TFT_RED);
        //_spr->setCursor(0, 0);
        _spr->fillSprite(TFT_BLACK);
    }

    void _onEndDraw() override
    {
        _spr->pushSprite(0, 0);
    }

    void drawProgressBar(ElProgressBar *progressBar) override
    {
    }

    void drawButton(ElTextButton *button) override
    {
    }

    void drawWaveform(ElWaveform *waveform) override
    {
    }

    void drawText(ElText *text) override
    {
        //_display->getFontForSize(text->getTextSize());
        _spr->setFreeFont(&FreeSans12pt7b);

        String title = text->getText();
        int16_t x = text->getX() + text->getParent()->getX();
        int16_t y = text->getY() + text->getParent()->getX();

        switch (text->getAlignment())
        {
        case el_text_align::EL_TEXT_ALIGN_CENTER_PARENT:
            x += (text->getParent()->getWidth() >> 1) - (_tft->textWidth(title) >> 1);
            break;

        case el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH:
            x += (text->getWidth() >> 1) - (_tft->textWidth(title) >> 1);
            break;

        case el_text_align::EL_TEXT_ALIGN_LEFT:
        default:
            break;
        }

        if (text->getVerticalAlignment() == el_vertical_align::EL_ALIGN_CENTER)
        {
            y += (text->getParent()->getHeight() >> 1) - (_tft->fontHeight() >> 1);
        }

        //_gfx->setCursor(0, 0);
        //_spr->drawPixel(10, 10, TFT_RED);
        _spr->setTextColor(TFT_WHITE);  // Устанавливаем цвет текста на белый
        _spr->setCursor(x, y);          // Устанавливаем курсор в верхний левый угол спрайта
        _spr->println(text->getText()); // Выводим текст в спрайте

        // _textEngine->setFont(_display->getFontForSize(text->getTextSize()));

        // String textTitle = text->getText();

        // int16_t x = text->getX();
        // int y = text->getY();

        // int w = _textEngine->getUTF8Width(textTitle.c_str());
        // int h = _textEngine->getFontAscent();

        // // Горизонтальное выравнивание
        // switch (text->getAlignment())
        // {
        // case el_text_align::EL_TEXT_ALIGN_CENTER_PARENT:
        //     x = _getTextCenterX(textTitle, text->getParent()->getX(), text->getParent()->getWidth());
        //     break;

        // case el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH:
        //     x = _getTextCenterX(textTitle, x + text->getParent()->getX(), text->getWidth());
        //     break;

        // case el_text_align::EL_TEXT_ALIGN_RIGHT:
        //     x = text->getWidth() - _textEngine->getUTF8Width(textTitle.c_str());
        //     break;
        // }

        // _textEngine->drawUTF8(x, y, textTitle.c_str());
    }

    void drawCenteredGroup(ElCenteredGroup *group) override
    {
        // for (auto el : group->getElements())
        // {
        //     drawElement(el);
        // }
    }

    void drawLine(ElLine *line) override
    {
    }

    void drawDisplayTest(ElDisplayTest *displayTest)
    {
        // _tft->drawPixel(10, 10, TFT_RED);
    }

    void drawBatteryIndicr(ElBattery *batteryIndcr) override
    {
    }
    void drawScrollbar(ElScrollBar *scrollbar) override
    {
    }

    uint8_t getMaxTextWidth(el_text_size textSize) override
    {
        return 10;
    }

    uint8_t getMaxTextHeight(el_text_size textSize) override
    {
        return 10;
    }
};
