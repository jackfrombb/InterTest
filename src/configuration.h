/* **********************************************
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino framework

In cooperatiion with svdpfaf (svddevelop@gmail.com)

defines для изменения конфигурации железа

- Все пункты отделены коментарием вида:
  //[Описание]------------------
- Один пункт - одна настройка - одна раскомментированная строка
- Почти все дефайны переключают инициализацию классов в main.cpp, но это не 100%
************************************************ */
#pragma once

// Номер версии. Отсчет начнется, когда будет готово для тестирования----------------
#define APP_VERSION 0.01

// Язык интерфейса-----------------------------------------
// #define LOCALE_RU
#define LOCALE_EN

// Выбор платы-----------------------------
// #define WROOM32
 #define S2MINI
// #define ESP32S

// Дисплей-----------------------------------
// #define NOKIA5110_ // Nokia PCD8544 display
// #define OLED128x64 //дисплей 0.96 OLED I2C
#define ST7735_TFT_128x160_1_8 // подключение дисплея на контроллере ST7735 TFT 128x160 (1.8, red plate with sd)

// Выбор способа упрвления-----------------
#define ENCODER
// #define KEYPAD // НЕ РЕАЛИЗОВАНО
// #define CONTROL_IR //приемник 1838. NEC. Подключаю по туториалу https://kit.alexgyver.ru/tutorials/ir-remote/

// Доп функции-----------------------------
// #define BUZZ_