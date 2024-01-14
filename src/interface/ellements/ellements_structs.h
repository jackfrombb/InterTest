#pragma once

#define ELEMENT_POSITION_CENTER -1

/// @brief Размер текста
typedef enum
{
    EL_TEXT_SIZE_SUPER_LARGE,
    EL_TEXT_SIZE_LARGE,
    EL_TEXT_SIZE_MIDDLE,
    EL_TEXT_SIZE_SMALL,
    EL_TEXT_SIZE_SUPER_SMALL
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
    EL_TYPE_UNKNOWN,
    EL_TYPE_BUTTON,
    EL_TYPE_TEXT,
    EL_TYPE_IMAGE,
    EL_TYPE_WAVEFORM,
    EL_TYPE_LINE,
    EL_TYPE_POINT,
    EL_TYPE_PROGRESS_BAR,
    EL_TYPE_GROUP,
    EL_TYPE_CENTERED_GROUP
} el_type;

typedef struct
{
    uint16_t width;
    uint16_t height;
} area_size;
