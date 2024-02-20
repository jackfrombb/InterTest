#pragma once

 // Кнопка. Можно менять фокус, если на странице нескольких кнопок, установив Id (setButtonId) для кнопки и меняя SelectedButtonPtr
 // Фокус на кнопке отмечается (напр. ракой вокруг кнопки)
#include "button.h"

// Текст. Можно отображать посимвольное редактирование setEditPosition
#include "el_text.h"

// Отрисовка графика/осциллограммы
#include "wavefrom.h"

// Отрисовка индикации прогресса
#include "progress_bar.h"

// Группа. Объединение нескольких элементов в одном массиве. Корневой элемент
#include "el_group.h"

// Отцентрованная группа.
#include "el_centered_group.h"

// Линия шириной в 1 пиксель
#include "el_line.h"

// Элемент для проверки дисплея и разных экспериментов
#include "el_display_test.h"

// Индикатор заряда. Основан на progress_bar.h
#include "el_battery.h"

// Индикатор прокрутки
#include "el_scrollbar.h"

// Не доделан. Вывод списка из групп элементов, отображающий только необходимые, видимые элементы
#include "el_recycler_group.h"
