/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment
************************************************ */
#include <Arduino.h>


static void copy(int *src, int *dst, int len)
{
  memcpy(dst, src, sizeof(src[0]) * len);
}
