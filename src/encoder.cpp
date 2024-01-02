
/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

************************************************ */

#include "common.h"
#include "esp32-hal-gpio.h"
#include "hard_timer.h"
#ifndef EXCLUDE_GIVER_
  #include <EncButton.h>
#endif
#include "display.h"
#include "helpers.h"
#include "oscil.h"


// Энкодер
#ifndef EXCLUDE_GIVER_
EncButton enc(ENC_DT, ENC_CLCK, ENC_SW);
#endif


// Частота генерации
int pwmF = 1000;

void loop_encoder(){
  #ifndef EXCLUDE_GIVER_
  enc.tick();
  #endif

}


/// @brief Прерывание для обработки пропущенных считываний энкодера
/// @param args = NULL
/// @return nothing
bool IRAM_ATTR encTick(void *args)
{
#ifndef EXCLUDE_GIVER_
  enc.tickISR();
#endif
  return false;  //объясни почему false???
}

HardTimer encoderTimer = HardTimer(encTick, TIMER_GROUP_1, TIMER_0, 600, 80);


 void encEvent()
 {
 #ifndef EXCLUDE_GIVER_
   // EB_PRESS - нажатие на кнопку
   // EB_HOLD - кнопка удержана
   // EB_STEP - импульсное удержание
   // EB_RELEASE - кнопка отпущена
   // EB_CLICK - одиночный клик
   // EB_CLICKS - сигнал о нескольких кликах
   // EB_TURN - поворот энкодера
   // EB_REL_HOLD - кнопка отпущена после удержания
   // EB_REL_HOLD_C - кнопка отпущена после удержания с предв. кликами
   // EB_REL_STEP - кнопка отпущена после степа
   // EB_REL_STEP_C - кнопка отпущена после степа с предв. кликами
   Serial.println("Enc action: " + String(enc.action()));
   switch (enc.action())
   {
   case EB_HOLD:
     settingsVal = range(settingsVal + enc.dir(), 0, 2, true);
     framesForMenuTitleTimer = millis();
     break;

   case EB_CLICK:
     Serial.println(oscil.oscilTimer.playPause() ? "Pause - Oscil" : "Play - Oscil");
     break;

   case EB_TURN:
     if (settingsVal == 0)
     {
       const int steep = 1;
       // uint64_t val = timerAlarmReadMicros(measureTimer) + steep * enc.dir();
       // timerAlarmWrite(measureTimer, val, true);
       // timer_pause(TIMER_GROUP_0, TIMER_1);
       // uint64_t alarm_value;
       // timer_get_alarm_value(TIMER_GROUP_0, TIMER_1, &alarm_value);
       // timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, alarm_value + steep * enc.dir());
       // timer_start(TIMER_GROUP_0, TIMER_1);
       auto alarm_value = oscil.oscilTimer.getTickTime() + (enc.dir() * steep);
       oscil.oscilTimer.setNewTickTime(alarm_value);
       Serial.println("Frq accur: " + String(alarm_value));
     }
     else if (settingsVal == 1)
     {
       const int steep = 100;
       auto alarm_value = oscil.oscilTimer.getTickTime() + (enc.dir() * steep);
       oscil.oscilTimer.setNewTickTime(alarm_value);
       Serial.println("Frq fast: " + String(alarm_value));
     }
     else if (settingsVal == 2)
     {
       pwmF = range(pwmF + enc.dir() * 100, 0, 200000);
       ledcSetup(3, pwmF, 8);
       Serial.println("PWM: " + String(pwmF));
     }
     break;
   }
 #endif
 }

void encEvent();

void setup_encoder(){
      // Запитываем энкодер и подключаем к нему событие
  #ifndef EXCLUDE_GIVER_
    pinMode(ENC_VCC, OUTPUT);
    digitalWrite(ENC_VCC, 1);
    enc.attach(encEvent);
    
    encoderTimer.init();
  #endif
}