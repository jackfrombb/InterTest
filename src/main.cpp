// ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>

// Все настройки железа здесь
#include "configuration.h"

// Основной буфер.
#define BUFFER_LENGTH 168

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

#include "interface/engines/interface_engine_u8g2.h"
InterfaceEngineVirtual *interfaceEngine = new InterfaceEngine_U8g2(&mainBoard);

bool interfaceDrawInProcess = false; // Флаг начала прорисовки интерфейса

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

TickType_t xLastWakeTime;
const TickType_t xFrequency = 50;
void drawInterfaceThread(void *pvParameters)
{
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    ElWaveform<uint16_t> *waveform = new ElWaveform<uint16_t>(oscil->getBuffer());
    waveform->setArea(display_area{.leftUp = point_t{
                                       .x = 0,
                                       .y = 0,
                                   },
                                   .rightDown = point_t{
                                    .x = mainBoard.getDisplay()->getResoluton().width, 
                                    .y = mainBoard.getDisplay()->getResoluton().height}
                                    });
    interfaceEngine->drawWaveform(waveform);
    delete waveform;
    // drawOscilograf(oscil->getBuffer());
    xTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(300);

  Serial.println("Start to config:");
  Serial.println("MainBoard");
  mainBoard.init();

  delay(300);

  Serial.println("Control");
  control_init();

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif

  Serial.println("PWM");
  // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcSetup(2, pwmF, 8);
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcWrite(2, 254 / 2);

  delay(300);

  Serial.println("Oscil");
  oscil->init();
  Serial.println("Voltmetr (not need it)");

  // Прикрепить процесс к ядру
  xTaskCreatePinnedToCore(
      drawInterfaceThread,   // Function to implement the task
      "drawInterfaceThread", // Name of the task
      2048,                  // Stack size in bytes
      NULL,                  // Task input parameter
      10,                    // Priority of the task
      NULL,                  // Task handle.
      tskNO_AFFINITY         // Core where the task should run
  );
}

void loop()
{
  control_loop();
}