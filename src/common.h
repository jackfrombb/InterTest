/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

************************************************ */
#pragma once

//#define EXCLUDE_OSCIL_ 1 
//#define EXCLUDE_HARDTIMER_ 1
//#define EXCLUDE_GIVER_ 1

//Выбор платы
#define WROOM32
//#define S2MINI

//Выбор способа упрвления
#define ENCODER
//#define KEYPAD

//Доп функции
//#define BUZZ

#ifdef S2MINI
// Nokia PCD8544 display
#define RST 2                       // Pin1 (RST)  GPIO2
#define CE 15                       // Pin2 (CE)  GPIO15
#define DC 4                        // Pin3 (DC)  GPIO4
#define DIN 17                      // Pin4 (Din)  GPIO17
#define CLK 18                      // Pin5 (Clk)  GPIO18
                                    // Pin6 (Vcc)  3.3V
#define DISPLAY_LED_PIN GPIO_NUM_10 // Pin7 (BL)
                                    // Pin8 (GND)  GND
#endif

#ifdef WROOM32
// Nokia PCD8544 display
#define RST GPIO_NUM_16                      // Pin1 (RST)  GPIO2
#define CE GPIO_NUM_17                       // Pin2 (CE)  GPIO15
#define DC GPIO_NUM_5                        // Pin3 (DC)  GPIO4
#define DIN GPIO_NUM_18                      // Pin4 (Din)  GPIO17
#define CLK GPIO_NUM_19                      // Pin5 (Clk)  GPIO18
//#define DISPLAY_VCC GPIO_NUM_21                         // Pin6 (Vcc)  3.3V
#define DISPLAY_LED_PIN GPIO_NUM_21          // Pin7 (BL)
                                             // Pin8 (GND)  GND
#endif

#ifdef ENCODER
#ifdef S2MINI
// Энкодер
#define ENC_VCC GPIO_NUM_38
#define ENC_CLCK GPIO_NUM_37
#define ENC_DT GPIO_NUM_39
#define ENC_SW GPIO_NUM_40 // Кнопка
#endif

#ifdef WROOM32
#define ENC_VCC GPIO_NUM_12
#define ENC_CLCK GPIO_NUM_26
#define ENC_DT GPIO_NUM_27
#define ENC_SW GPIO_NUM_14 // Кнопка
#endif
#endif


#ifdef KEYPAD
#define KEY_DELAY_PERIOD           400 // Supress of key jitter with ms_delay

#define PIN_KEYPAD_KEY1             16 
#define PIN_KEYPAD_KEY2             17 
#define PIN_KEYPAD_KEY3             19
#define PIN_KEYPAD_KEY4             18
#define PIN_KEYPAD_KEYSHUNT         18

#define PIN_MC_A7                   13
#endif

#ifdef BUZZ
#define PIN_MK_BUZZ                 21
#define MK_BUZZ_ACTIVE              LOW
#define MK_BUZZ_INACTIVE            HIGH
#endif


