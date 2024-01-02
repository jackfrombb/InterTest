/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

************************************************ */

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>
#include "oscil.h"

extern U8G2 u8g2;

extern int settingsVal;
extern ulong framesForMenuTitleTimer;

extern Oscilloscope oscil ;

extern void     setup_display();
extern void     lopp_display();
extern uint32_t readAnalogVal();
extern void     drawBack();
extern void     drawValues(int32_t a_buf[]);
