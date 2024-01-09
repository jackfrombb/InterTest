// ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>

// Переключение железа здесь
#include "configuration.h"

// Вспомогательные методы общие
#include "helpers.h"
// Вспомогательные структуры дисплея
#include "displays/display_structs.h"

// Логика осцилографа
#include "oscils/oscil.h"
#include "oscils/oscil_i2s.h"
#include "oscils/oscil_virtual.h"
// Логика вольтметра
#include "voltmeter.h"
// Логика тамера прерываний
#include "hard_timer.h"

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2s.h"

#include "interface/ellements/ellements_list.h"
#include "interface/engines/interface_engine.h"
#include "interface/interface_controller.h"

// Пищалка
#ifdef BUZZ
#include "buzzer.h"
#endif

// Определение контроллера
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

MainBoard mainBoard(&adcInfo, display);

#ifdef U8G2_ENGINE
#include "interface/engines/interface_engine_u8g2.h"
InterfaceEngineVirtual *interfaceEngine = new InterfaceEngine_U8g2(&mainBoard);
#elif defined(ADAFRUIT_ENGINE)
//In process
#endif

InterfaceController interfaceController(display, interfaceEngine);


// new OscilAdc(&mainBoard, 8402);
// OscilI2s(&mainBoard, (uint32_t) 10000);
OscilVirtual *oscil = new OscilI2s(&mainBoard, (uint32_t)50000); // board_readAnalogVal - определяется в файле board_***.h
Voltmetr voltmetr = Voltmetr(&mainBoard);

int settingsVal = 0;               // 0 - Частота опроса, 1 - частота кадров, 2 - частота шима
const float maxMeasureValue = 3.2; // Потолок по напряжению, если ниже 3.0 то ломается. Больше можно
ulong framesForMenuTitleTimer = 0; // Счетчик кадров для отображения названия меню, его увеличивает control, а отслеживает interface

// Частота генерации
int pwmF = 100000;

#ifdef ENCODER
#include "controls/control_encoder.h"
#elif defined(KEYPAD)
#include "controls/control_keypad.h"
#endif

#include "displays/display_helper.h"

void setup()
{
  Serial.begin(115200);
  delay(300);

  Serial.println("Start to config:");
  Serial.println("Main board");
  mainBoard.init();
  Serial.println("Interface controller");
  interfaceController.init();
  
  float progress = 0.10;
  interfaceController.showHelloPage(&progress);

  delay(300);

  Serial.println("Control");
  control_init();
  
  progress = 0.3;

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif
  progress = 0.6;

  Serial.println("PWM");
  // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcSetup(2, pwmF, 8);
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcWrite(2, 254 / 2);

  progress = 0.8;

  Serial.println("Oscil");
  oscil->init();
  Serial.println("Voltmetr (not need it)");

  progress = 1.0;
}

void loop()
{
  control_loop();
}