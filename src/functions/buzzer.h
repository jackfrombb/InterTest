/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

Communication with encoder. Depended with the library from Alex Giver.

************************************************ */
#pragma once
#include <Arduino.h>

#ifdef BUZZ
#define PIN_MK_BUZZ                 21
#define MK_BUZZ_ACTIVE              LOW
#define MK_BUZZ_INACTIVE            HIGH
#endif

extern void setup_buzzer();
extern void do_buzz( int a_period );