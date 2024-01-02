//ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>
//  USE_SSD1306 // Use I2C OLED screen on SSD1306 chipset

#include "common.h"
#define BUFFER_LENGTH 168

#ifndef  EXCLUDE_OSCIL_
   #include "oscil.h"
#endif
#ifndef  EXCLUDE_HARDTIMER_
   #include "hard_timer.h"
#endif

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "helpers.h"
#include "display.h"


#ifdef BUZZER_
#include "buzzer.h"
#endif


#ifdef KEYPAD_
  #include "keypad.h"
#endif

#include <driver/adc.h>

#ifdef ENCODER_
  #include "encoder.h"
#endif

void setup()
{

  setup_display();



  #ifdef ENCODER_
    setup_encoder();
  #endif

  #ifdef KEYPAD_
    setup_keypad();
  #endif

  #ifdef BUZZ_
    setup_buzzer();
  #endif

   // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcSetup(2, pwmF, 8);
  ledcWrite(2, 254 / 2);


  Serial.begin(9600);
  delay(1000);

 
}

void loop()
{
  #ifdef ENCODER_
  loop_encoder();
  #endif

  #ifdef KEYPAD_
  loop_keypad();
  #endif

  lopp_display();

}