/* **********************************************
Author: JackFromBB - jack@boringbar.ru /
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino framework

In cooperatiion with svdpfaf (svddevelop@gmail.com)

defines для изменения конфигурации железа

************************************************ */
#pragma once

#define APP_VERSION 0.01

// #define EXCLUDE_OSCIL_ 1
// #define EXCLUDE_HARDTIMER_ 1

// Выбор платы
// #define WROOM32
#define S2MINI

// Выбор способа упрвления
//#define ENCODER
//#define KEYPAD // НЕ РЕАЛИЗОВАНО
#define CONTROL_IR //приемник 1838. NEC. Подключаю по туториалу https://kit.alexgyver.ru/tutorials/ir-remote/

// Доп функции
// #define BUZZ_

// #define NOKIA5110_ // Nokia PCD8544 display
// #define OLED128x32_ //дисплей 0.96 OLED I2C
#define ST7735_TFT_128x160_1_8 // подключение дисплея на контроллере ST7735 TFT 128x160 (1.8, red plate with sd)

// Тип интерфейса
#define WIDE_UI // Для экранов близких к квадрату
// #define SLIM_UI // Для продолговатых экранов (НЕ РЕАЛИЗОВАНО)

// Библиотека подключения дисплея
#define U8G2_ENGINE
// #define ADAFRUIT_ENGINE //НЕ РЕАЛИЗОВАНО