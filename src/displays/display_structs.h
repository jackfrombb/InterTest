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

// Точка на дисплее
typedef struct
{
  int x;
  int y;
} point_t;

/// @brief Участок дисплея
typedef struct display_position
{
  point_t leftUp;    // Левая верхняя точка
  point_t rightDown; // Правая нижняя точка

  int getX() const
  {
    return leftUp.x;
  }

  int getY() const
  {
    return leftUp.y;
  }

  void setX(int x)
  {
    int diff = x - leftUp.x;
    leftUp.x = x;
    rightDown.x += diff;
  }

  void setY(int y)
  {
    int diff = y - leftUp.y;
    leftUp.y = y;
    rightDown.y += diff;
  }

  /// @brief Ширина участка на дисплее
  /// @return ширина в пикселях
  int getWidth() const
  {
    return rightDown.x - leftUp.x;
  }

  void setWidth(uint32_t width)
  {
    rightDown.x = leftUp.x + width;
  }

  /// @brief Высота участка на дисплее
  /// @return высота в пикселях
  int getHeight() const
  {
    return rightDown.y - leftUp.y;
  }

  void setHeight(uint32_t height)
  {
    rightDown.y = leftUp.y + height;
  }

  bool operator!=(const display_position &another) const
  {
    if (getX() == another.getX() && getY() == another.getY() && getWidth() == another.getWidth() && getHeight() == another.getHeight())
    {
      return false;
    }

    return true;
  }

  bool operator==(const display_position &another) const
  {
    if (getX() == another.getX() && getY() == another.getY() && getWidth() == another.getWidth() && getHeight() == another.getHeight())
    {
      return true;
    }

    return false;
  }

} display_position;

// Контейнер для хранения размеров чего либо
typedef struct
{
  uint16_t width;
  uint16_t height;
} area_size;