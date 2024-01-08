/**
 * @file display_structs.h
 * @author jackfrombb (jack@boringbar.com)
 * @brief Вспомогательные структуры для дисплея
 * @version 0.1
 * @date 2024-01-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include <Arduino.h>

/// @brief Тип подключения дисплея
typedef enum
{
    DISPLAY_TYPE_UNKNOWN,
    DISPLAY_TYPE_SPI,
    DISPLAY_TYPE_I2C
} display_type;

/// @brief Тип библиотеки для вывода информации на  дисплей
typedef enum
{
    DISPLAY_LIB_UNKNOWN,
    DISPLAY_LIB_U8G2,
} display_library;

/// @brief Тип интерфейса
typedef enum
{
    DISPLAY_INTERFACE_TYPE_UNKNOWN,
    DISPLAY_INTERFACE_TYPE_WIDE,
    DISPLAY_INTERFACE_TYPE_SLIM,
} display_interface_type;

/// @brief Разрешение дисплея
typedef struct
{
    uint16_t width;
    uint16_t height;
} display_resolution;

// Точка на дисплее
typedef struct
{
  int x;
  int y;
} point_t;

/// @brief Участок дисплея
typedef struct display_area
{
  point_t leftUp; //Левая верхняя точка
  point_t rightDown; //Правая нижняя точка

  int getX() const{
    return leftUp.x;
  }

  int getY() const{
    return leftUp.y;
  }

  /// @brief Ширина участка на дисплее
  /// @return ширина в пикселях
  int getWidth() const{
    return rightDown.x - leftUp.x;
  }

  /// @brief Высота участка на дисплее
  /// @return высота в пикселях
  int getHeight() const{
    return rightDown.y - leftUp.y;
  }
  
} display_area;

