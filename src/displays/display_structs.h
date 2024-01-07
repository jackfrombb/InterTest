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

/// @brief Зона дисплея
typedef struct
{
  point_t leftUp; //Левая верхняя точка
  point_t rightDown; //Правая нижняя точка
} display_area;
