#pragma once

#include "ellement_virtual.h"

class ElText : public ElementVirtual
{
private:
    // Отображаемый текст
    String _text;
    // Размер текста
    el_text_size _elSize = el_text_size::EL_TEXT_SIZE_MIDDLE;
    // Положение текста
    el_text_align _alignment = el_text_align::EL_TEXT_ALIGN_LEFT;

    // Динамически вычисляемый текст (аргументы для вычисления и функция вычисления)
    void *_calculateTextArgs = nullptr;
    std::function<String(void *)> _calculateText = nullptr;

    // Флаг режима посимвольного редактирования (для числовых значений начинающихся с начала текста n*)
    bool _inEditMode = false;
    // Положение редактирования (отсчет положения идет от 1, до переделок это было важно, при необходимости переделаю)
    int8_t _editPosition = -1;

    // Аргументы передаваемые вместе с событием редактирования и запроса числа
    void *_editEventArgs = nullptr;
    // Получить редактируемое число
    function<int(void* args)> _getEditNumber;
    // Событие редактирования возвращает true если редактирование удачно иначе число не меняется
    // отправляет новое число, элемент в котором происходит редактирование и аргументы
    function<bool(int val, ElText *el, void *args)> _onEditEvent = nullptr;

    /// @brief Проверить положение позиции редактирования числа
    /// @param changeValue величина изменения позиции (+1, -1)
    /// @param maxPos максимальная позиция в числе
    void _checkEditPosition(int8_t changeValue, uint8_t maxPos)
    {
        _editPosition = range(_editPosition + changeValue, 1, maxPos, true); // Переносим на следующий разряд, ограничивая максимальным и минимальным значением
        logi::p("ELText", "max edit pos: " + String(maxPos) + " after val: " + String(_editPosition));
    }

    /// @brief Изменить редактируемое число
    /// @param increase увелисть или уменьшить
    /// @return редактированное число
    int32_t _changeEditNumber(bool increase)
    {
        int editNumber = _getEditNumber(_editEventArgs); // преобразуем число для удобства

        uint16_t maxPos = getMaxNumPosition<uint32_t>(editNumber);   // Узнаем максимальную позицию
        int num = pow(10, maxPos - _editPosition);                   // Вычисляем величину изменения, возводя 10 в степень (10^0=1, 10^1=10, 10^n=1n)
        editNumber = increase ? editNumber + num : editNumber - num; // Вычисляем новое значение частоты

        // Устанавливаем значение частоты опроса в класс осциллографа
        // Он самостоятельно ограничивает минимальную и максимальную частоту
        bool isSuccessful = _onEditEvent(editNumber, this, _editEventArgs);

        // Узнаем максимальную позицию после изменения числа
        uint16_t maxPosAfter = getMaxNumPosition<uint32_t>(_getEditNumber(_editEventArgs));

        // Вычисляем разницу
        int16_t diff = maxPosAfter - maxPos;

        // Проверяем, что число позиция ещё входит в необходимые пределы
        // и переносим указатель на другую позицию если изминилось кол-во разрядов
        _checkEditPosition(diff, maxPosAfter);

        return editNumber;
    }

public:
    ElText() = default;

    explicit ElText(String text)
    {
        setText(std::move(text));
    }

    ~ElText() = default;

    ElText *setText(String text)
    {
        _text = std::move(text);
        return this;
    }

    ElText *setCalculatedText(std::function<String(void *)> calculateText, void *args = nullptr)
    {
        _calculateText = std::move(calculateText);
        _calculateTextArgs = args;
        return this;
    }

    String getText()
    {
        if (_calculateText != nullptr)
            return _calculateText(_calculateTextArgs);
        else
            return _text;
    }

    String *getTextPtr()
    {
        return &_text;
    }

    virtual ElText *setPosition(int x, int y)
    {
        setArea(display_position{.leftUp{
            .x = x,
            .y = y,
        }});

        return this;
    }

    /// @brief Получить номер редактиркемого символа (отсчет от 1, 1 = первый символ в строке)
    int8_t getEditPosition()
    {
        return _editPosition;
    }

    /// @brief Получить размер текста
    /// @return Размер текста в перечислении el_text_size
    virtual el_text_size getTextSize()
    {
        return _elSize;
    }

    /// Установить размер текста
    virtual ElText *setTextSize(el_text_size size)
    {
        _elSize = size;
        return this;
    }

    virtual el_text_align getAlignment()
    {
        return _alignment;
    }

    virtual ElText *setAlignment(el_text_align alignment)
    {
        _alignment = alignment;
        return this;
    }

    virtual bool switchEditMode()
    {
        _inEditMode = !_inEditMode;

        if (_inEditMode)
        {
            // Вычисляем положение редактирования
            _editPosition = getMaxNumPosition<int32_t>(_getEditNumber(_editEventArgs));
        }
        else
        {
            // Убираем положение редактирования
            _editPosition = -1;
        }

        return _inEditMode;
    }

    bool isInEditMode()
    {
        return _inEditMode;
    }

    virtual bool onControl(control_event_type event, void *args = nullptr) override
    {
        if (_inEditMode)
        {
            switch (event)
            {
            case control_event_type::PRESS_OK:
            {
                int8_t maxPos = getMaxNumPosition<int32_t>(_getEditNumber(_editEventArgs)); // Максимальное положение
                _checkEditPosition(- 1, maxPos);
                break;
            }

            case control_event_type::PRESS_LEFT:
                // Уменьшить число
                _changeEditNumber(false);
                break;

            case control_event_type::PRESS_RIGHT:
                // Увеличить число
                _changeEditNumber(true);
                break;

            case control_event_type::PRESS_BACK:
                // Выйти из режима редактирования
                switchEditMode();
                break;
            }
        }
        return true;
    }

    /// @brief Установить слушателя события посимвольного редактирования
    /// @param getEditNumber метод для получения редактируемого числа
    /// @param onEditEvent обработчик события
    /// @param args аргументы передаваемые в обработчик (возвращает true если число удачно установлено)
    virtual ElText *
    setOnEditModeEvent(function<int(void* args)> getEditNumber,
                       function<bool(int val, ElText *el, void *args)> onEditEvent,
                       void *args = nullptr)
    {
        _getEditNumber = getEditNumber;
        _onEditEvent = onEditEvent;
        _editEventArgs = args;
        return this;
    }

    bool *getInEditModePtr()
    {
        return &_inEditMode;
    }

    el_type getElementType() override
    {
        return EL_TYPE_TEXT;
    }
};