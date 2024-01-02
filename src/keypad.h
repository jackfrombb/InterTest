/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

Communication with encoder. Depended with the library from Alex Giver.

Keys processing

************************************************ */
#pragma once

#include <Arduino.h>

#ifdef KEYPAD_

    typedef struct {
        uint8_t key1:1;
        uint8_t key2:1;
        uint8_t key3:1;
        //uint8_t key4:1;
        uint8_t keyshunt:1;
    } keys_t;

    typedef union {   
        uint8_t as_int;
        keys_t as_keys;
    } keypad_t;
    

    extern keypad_t Keypad;
    extern void setup_keypad();
    extern void loop_keypad();

#endif