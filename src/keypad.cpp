/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

Communication with encoder. Depended with the library from Alex Giver.

************************************************ */


#include "common.h"
#include "keypad.h"
#ifdef KEYPAD_

    keypad_t Keypad;

    void setup_keypad(){

        Keypad.as_int = 0;

        pinMode(PIN_KEYPAD_KEY1, INPUT_PULLUP);
        pinMode(PIN_KEYPAD_KEY2, INPUT_PULLUP);
        pinMode(PIN_KEYPAD_KEY3, INPUT_PULLUP);
        pinMode(PIN_KEYPAD_KEYSHUNT, INPUT_PULLUP);

    }


    void loop_keypad(){

        keypad_t tmp;
        tmp.as_int = 0;
        tmp.as_keys.key1 = digitalRead( PIN_KEYPAD_KEY1 );
        tmp.as_keys.key2 = digitalRead( PIN_KEYPAD_KEY2 );
        tmp.as_keys.key3 = digitalRead( PIN_KEYPAD_KEY3 );
        tmp.as_keys.keyshunt = digitalRead( PIN_KEYPAD_KEYSHUNT );

        if ( tmp.as_int != Keypad.as_int ){

            Serial.printf_P(PSTR("Keypad:%d>:"), Keypad.as_int );
            Serial.println( tmp.as_int );
            Keypad.as_int = tmp.as_int;
            delay( KEY_DELAY_PERIOD );
        }

    }
#endif