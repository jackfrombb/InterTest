#pragma once

#define ELEMENT_POSITION_CENTER -1

/// @brief Размер текста
typedef enum
{
    EL_VOLTMETER_VALUE_LARGE,
    EL_TEXT_SIZE_SUPER_LARGE,
    EL_TEXT_SIZE_LARGE,
    EL_TEXT_SIZE_MIDDLE,
    EL_TEXT_SIZE_SMALL,
    EL_TEXT_SIZE_SUPER_SMALL,
} el_text_size;

/// @brief Выравнивание текста
typedef enum
{
    EL_TEXT_ALIGN_RIGHT,
    EL_TEXT_ALIGN_LEFT,
    EL_TEXT_ALIGN_CENTER,
} el_text_align;

/// @brief Выравнивание по вертикали
typedef enum
{
    EL_ALGN_TOP,
    EL_ALIGN_CENTER,
    EL_ALIGN_BOTTOM
} el_vertical_align;

/// @brief Тип эллемента
typedef enum
{
    EL_TYPE_UNKNOWN,          // Пустой
    EL_TYPE_BUTTON,           // Кнопка, по сути текствое поле которое можно выделить
    EL_TYPE_TEXT,             // Текст
    EL_TYPE_IMAGE,            // Картинка (не реализовано)
    EL_TYPE_WAVEFORM,         // Отображение осциллограммы из буфера uint16_t значений
    EL_TYPE_LINE,             // Линия
    EL_TYPE_POINT,            // Точка, просто точка и точка.
    EL_TYPE_PROGRESS_BAR,     // Визуальное отображение прогресса какого либо процесса
    EL_TYPE_GROUP,            // Родительский контейнер, все эллементы отсчитывают свое положение от левого верхнего угла
    EL_TYPE_CENTERED_GROUP,   // Контейнер в котором все элементы расположены горизонтально, занимая ШИРИНА / кол-во эллементов пространство
    EL_TYPE_DISPLAY_TEST,     // Для экспериментов и проверки дисплея
    EL_TYPE_BATTERY_INDCATOR, // Отображать заряд батареи
    EL_TYPE_SCROLLBAR,        // Индикатор прокрутки страницы
} el_type;

// Контейнер для хранения размеров чего либо
typedef struct
{
    uint16_t width;
    uint16_t height;
} area_size;
