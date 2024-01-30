// ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>

#include "app_data.h"

// Переключение железа здесь
#include "configuration.h"
#include "logi.h"
#include "driver/spi_master.h"

// Вспомогательные методы общие
#include "helpers.h"
// Вспомогательные структуры дисплея
#include "displays/display_structs.h"

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"

// Логика осцилографа
#include "oscils/oscils_list.h"
// Логика вольтметра
#include "voltmeter.h"
// Логика тамера прерываний
#include "hard_timer.h"
// Логика генератора сигналов
#include "signal_generator.h"

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
ControlVirtual *control = new ControlEncoder();
#elif defined(KEYPAD)
// Не реализовано, нет панели с кнопками
#elif defined(CONTROL_IR)
// Для тестов
#include "controls/control_ir.h"
ControlVirtual *control = new ControlIr();
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

#ifdef S2MINI
#include "boards/esp32_s2mini.h"
MainBoard* mainBoard = new Esp32S2Mini(display, control);
#elif defined(WROOM32)
#include "boards/esp32_wroom32.h"
MainBoard* mainBoard = new Esp32Wroom32(display, control);
#endif

#ifdef U8G2_ENGINE
#include "interface/engines/interface_engine_u8g2.h"
InterfaceEngineVirtual *interfaceEngine = new InterfaceEngine_U8g2(mainBoard);
#elif defined(ADAFRUIT_ENGINE)
// In process
#endif

InterfaceController interfaceController(mainBoard, interfaceEngine);

// Частота генерации
int pwmF = 70000;

SignalGenerator sigGen(mainBoard->getPwmPin());

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  // Просто смотрю на конфигурацию в теории
  // spi_bus_config_t spi_config = {
  //   .data0_io_num = GPIO_NUM_0,
  //   .data1_io_num = GPIO_NUM_1,
  //   .data2_io_num = GPIO_NUM_2,
  //   .data3_io_num = GPIO_NUM_3,
  //   .data4_io_num = GPIO_NUM_5,
  //   .data5_io_num = GPIO_NUM_6,
  //   .data6_io_num = GPIO_NUM_7,
  //   .data7_io_num = GPIO_NUM_8,
  //   .max_transfer_sz = 0,
  //   .flags = 0,
  //   .intr_flags =0,
  // };

  // spi_bus_initialize(spi_host_device_t::SPI1_HOST, &spi_config, SPI_DMA_CH_AUTO);
  // spi_bus_add_device(spi_host_device_t::SPI1_HOST, )

  logi::p("Main", "Start");

  display->init();
  Serial.println("Display init OK");
  delay(100);

  mainBoard->init();
  delay(100);

  interfaceController.init();
  float *progress = interfaceController.showHelloPage();

  *progress = 0.3;
  delay(100);

  logi::p("Main", "Model: " + String(ESP.getChipModel()) +
                      "\nCores: " + String(ESP.getChipCores()) +
                      "\nCore Freq: " + String(ESP.getCpuFreqMHz()) +
                      "\nHeap: " + String(ESP.getHeapSize()));

  AppData::begin();

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif

  *progress = 0.6;
  delay(300);

  // Временный костыль для проверки АЦП
  //sigGen.startMeandrLedc(pwmF, 0.5);
  //sigGen.startMenadrDac(pwmF, .5);
  sigGen.startWaveDac(pwmF);

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