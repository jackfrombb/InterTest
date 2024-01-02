/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

Communication with encoder. Depended with the library from Alex Giver.

************************************************ */
#pragma once


#ifndef EXCLUDE_GIVER_
  #include <EncButton.h>
#endif
#include "hard_timer.h"

extern HardTimer encoderTimer;

extern float vRef;

#ifndef EXCLUDE_GIVER_
extern EncButton enc;
#endif


extern void encEvent();
extern void setup_encoder();
extern void loop_encoder();


