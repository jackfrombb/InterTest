// ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>

// Переключение железа здесь
#include "configuration.h"
#include "logi.h"

// Вспомогательные методы общие
#include "helpers.h"
// Вспомогательные структуры дисплея
#include "displays/display_structs.h"

// Логика осцилографа
#include "oscils/oscils_list.h"
// Логика вольтметра
#include "voltmeter.h"
// Логика тамера прерываний
#include "hard_timer.h"

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2s.h"

// Определяем UI
#ifdef WIDE_UI
#include "interface/pages/views/wide_screen/wide_views_list.h"
#elif defined(SLIM_UI)
#include "interface/pages/views/slim_screen/slim_views_list.h"
#endif

#include "interface/ellements/ellements_list.h"
#include "interface/engines/interface_engine.h"
#include "interface/interface_controller.h"
#include "displays/display_helper.h"
#include "controls/control_virtual.h"

// Пищалка
#ifdef BUZZ
#include "buzzer.h"
#endif

#ifdef ENCODER
#include "controls/control_encoder.h"
ControlVirtual* control = new ControlEncoder();
#elif defined (KEYPAD)
#endif

// Определение АЦП в зависимости от платы
#ifdef S2MINI
init_adc_info adcInfo = {
    .unit = ADC_UNIT_1,
    .chanelAdc1 = ADC1_CHANNEL_0,
    .atten = ADC_ATTEN_11db,
    .width = ADC_WIDTH_13Bit,
};

#elif defined(WROOM32)
init_adc_info adcInfo = {
    .unit = ADC_UNIT_1,
    .chanelAdc1 = ADC1_CHANNEL_0,
    .atten = ADC_ATTEN_11db,
    .width = ADC_WIDTH_12Bit,
};
#endif

// Определение дисплея
#ifdef NOKIA5110_
// Nokia PCD8544 display
#include "displays/display_nokia_5110.h"
DisplayVirtual *display = new Nokia5110_U8g2();
#elif defined(OLED128x32_)
// дисплей 0.96 OLED I2C
#include "displays/display_128x32.h"
DisplayVirtual *display = new Display128x64_U8g2();
#endif

MainBoard mainBoard(adcInfo, display, control);

#ifdef U8G2_ENGINE
InterfaceEngineVirtual *interfaceEngine = new InterfaceEngine_U8g2(&mainBoard);
#elif defined(ADAFRUIT_ENGINE)
// In process
#endif

InterfaceController interfaceController(&mainBoard, interfaceEngine);

// Частота генерации
int pwmF = 160000;

void setup()
{
  Serial.begin(115200);
  logi::p("Main", "Start");

  delay(100);

  mainBoard.init();
  delay(100);
  
  interfaceController.init();
  float *progress = interfaceController.showHelloPage();

  *progress = 0.3;
  delay(100);

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif

  *progress = 0.6;
  delay(300);


  // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcSetup(2, pwmF, 8);
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcWrite(2, 50);


  *progress = 0.8;
  delay(100);

  control->init();

  *progress = 0.9;
  delay(100);

  interfaceController.start();
}

void loop()
{
  control->loop();
}