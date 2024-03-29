#pragma once
#include <Arduino.h>
#include "configuration.h"

#define KEY_DELAY_PERIOD           400 // Supress of key jitter with ms_delay

#define PIN_KEYPAD_KEY1             16 
#define PIN_KEYPAD_KEY2             17 
#define PIN_KEYPAD_KEY3             19
#define PIN_KEYPAD_KEY4             18
#define PIN_KEYPAD_KEYSHUNT         18

#define PIN_MC_A7                   13

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

keypad_t Keypad;

void control_init(){
    Keypad.as_int = 0;

    pinMode(PIN_KEYPAD_KEY1, INPUT_PULLUP);
    pinMode(PIN_KEYPAD_KEY2, INPUT_PULLUP);
    pinMode(PIN_KEYPAD_KEY3, INPUT_PULLUP);
    pinMode(PIN_KEYPAD_KEYSHUNT, INPUT_PULLUP);
}

void control_loop(){
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