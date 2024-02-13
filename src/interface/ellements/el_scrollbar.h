#pragma once

class ElScrollBar : public ElementVirtual
{
private:
    float _position;
    float _displayedWidth;

public:
    ElScrollBar()
    {
    }

    /// @brief Получаить положение относительно общей ширины
    /// @return [min = 0.0; max = 1.0]
    float getScrollPosition()
    {
        return _position;
    }

    /// @brief Получить ширину отображаемого контента относительно общей ширины
    /// @return [min = 0.0; max = 1.0]
    float getDisplayedWidth()
    {
        return _displayedWidth;
    }

    /// @brief Установить положение и ширину индикатора прокрутки
    /// @param totalWidth Общая ширина всех эллементов прокрутки
    /// @param xPos Положение по x
    /// @param displayedWidth Отображаемая ширина
    void setScrollPosition(uint32_t totalWidth, uint32_t xPos, uint16_t displayedWidth)
    {
        totalWidth = totalWidth == 0 ? 1 : totalWidth;        // Исключаем деление на 0
        _displayedWidth = (float)displayedWidth / (totalWidth + displayedWidth); // Высчитваем относительную ширину, макс 1.0
        _position = ((float)xPos / (totalWidth + displayedWidth));               // Высчитываем относительную позицию, макс 1.0 - относительная ширина

        // Serial.println("w: " + String(_displayedWidth) + " p: " + String(_position));
    }

    el_type getElementType() override
    {
        return el_type::EL_TYPE_SCROLLBAR;
    }
};