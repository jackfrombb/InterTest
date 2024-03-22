#include <Arduino.h>
/**
 * @file main.cpp
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Прошивка для самодельного минимультиметра.
 * Весь платформозависимый код должен быть спрятан за интерфейсами или, на крайний случай, define'ами, для возможности добавления новго железа
 * 
 * Поддержка железа на данный момент:
 * - Микроконтроллеры: esp32(wroom32, s2mini, esp32s)
 * - Дисплеи: Nokia5110, oled, ST7735 TFT 128x160 (1.8, red plate with sd) [последний в ранней стадии добавления, но уже запускается]
 * (монохромные дисплеи на u8g2 довольно лего добавляются, главное что бы интерфейс вмещался)
 * @version 0.1
 * @date 2024-02-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// Переключение настроек здесь
#include "configuration.h"

// Переключение языка интерфейса. Все языкозависимые переменные интерфейса определять там и дублировать для каждой локали
#ifdef LOCALE_RU
#include "interface/localisation/ru.h"
#elif defined(LOCALE_EN)
#include "interface/localisation/en.h"
#endif

#include "app_data.h" // Тут чтение и запись настроек в ПЗУ
#include "logi.h"     // Централизация логирования
#include "USB.h"

#include "helpers.h"                  // Вспомогательные методы общие (найти средне)
#include "displays/display_structs.h" // Вспомогательные структуры дисплея (точка, область и пр)
#include "share_setting.h"            // Единый вид настроек. Класс помогающий передать настройку чего либо от разных компонентов
                                      // на страницу настроек.

#include "module_virtual.h" // Абстракция обобщающая инициализируемые компоненты (перестал применять еще в начале разработки, но зависимости не удалял)

#include "controls/control_virtual.h" // Абстракция обобщающая способы управления (кнопки, энкодеры и пр)

#include "interface/ellements/ellements_structs.h" // Вспомогательные структуры интерфейсных элементов (положение на экране, размеры и пр.)
#include "interface/ellements/ellement_virtual.h"  // Абстракция интерфейсных элементов

#include "displays/display_virtual.h"           // Абстракция дисплея. Cоздаёт двигатель отрисовки (InterfaceEngine прототипирован без реализации)
#include "interface/ellements/ellements_list.h" // Список элементов интерфейса (текстовые поля, кнопки, отображение графика и пр)
#include "interface/engines/interface_engine.h" // Абстракция двигателя отрисовки дизайна на дисплее

#include "controllers/pwm_virtual.h" // Абстракция логики генеатора
#include "controllers/adc_virtual.h" // Абстракция контроллера adc
#include "boards/board_virtual.h"           // Абстракция главной платы/контроллера
#include "oscils/oscil_virtual.h"    // Абстракция над логикой осциллографа
#include "oscils/oscil_logic.h"      // Основная логика считывания осциллограммы

#include "hard_timer.h"       // Логика тамера прерываний
#include "functions/voltmeter.h" // Логика вольтметра

#include "interface/pages/views/page_view.h"      // Абстракция представления страницы граф. интерфейса
#include "interface/pages/page_virtual.h"         // Абстракция над контроллера странички
#include "interface/ellements/ellement_virtual.h" // Абстракция элемента графического дизайна

#ifdef BUZZ
#include "buzzer.h" // Пищалка
#endif

#ifdef ENCODER
// Основной способ управления
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
// Определение интерфейса
#include "interface/pages/views/wide_screen/wide_views_list.h"
// Nokia PCD8544 display
#include "displays/display_nokia_5110.h"
DisplayVirtual *display = new Nokia5110_U8g2();
#elif defined(OLED128x64)
// Определение интерфейса
#include "interface/pages/views/wide_screen/wide_views_list.h"
// дисплей 0.96 OLED I2C
#include "displays/display_128x64.h"
DisplayVirtual *display = new Display128x64_U8g2();
#elif defined(ST7735_TFT_128x160_1_8)
// Определение интерфейса
#include "interface/pages/views/wide_screen/wide_views_list.h"
// подключение дисплея на контроллере ST7735 TFT 128x160 (1.8, red plate with sd)
#include "displays/display_128_160_spi_1_8_color.h"
DisplayVirtual *display = new Display128x160_1_8_Spi_Color();
#endif

#include "interface/pages/page_list.h"      // Абстракция над контроллера странички
#include "interface/interface_controller.h" // Управление страницами и отрисовкой


// Определение платы
#ifdef S2MINI
#include "boards/esp32_s2mini.h"
MainBoard *mainBoard = new Esp32S2Mini(display, control);
#elif defined(ESP32S)
#include "boards/esp32s.h"
MainBoard *mainBoard = new Esp32S(display, control);
#elif defined(WROOM32)
#include "boards/esp32_wroom32.h"
MainBoard *mainBoard = new Esp32Wroom32(display, control);
#endif

InterfaceController interfaceController(mainBoard);

void setup()
{
  Serial.begin(115200);

  // while (!Serial) // Эта строка не дает загрузится устройству пока не запустится Serial костыль для отладки s2mini
  // {
  //   vTaskDelay(1000); // Что бы не будить псов
  // };
  // Serial.println("SERIAL: Setup complete");

  logi::p("Main", "Start");

  // Инициализация синглтона хранения настроек/состояний
  AppData::begin();

  display->init();
  logi::p("Main", "Display init OK");
  delay(100);

  mainBoard->init();
  delay(100);

  interfaceController.init();
  float *progress = interfaceController.showHelloPage();

  *progress = 0.3;
  delay(100);

  // Выводим описание контроллера
  logi::p("Main", "Model: " + String(ESP.getChipModel()) +
                      "\nCores: " + String(ESP.getChipCores()) +
                      "\nCore Freq: " + String(ESP.getCpuFreqMHz()) +
                      "\nHeap: " + String(ESP.getHeapSize()));

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif

  *progress = 0.6;
  delay(300);

  // Тут должна быть полноценная калибровка, но пока нет реле, 
  // которое бы отключало иглу при включении, там просто устанавлвается значение делителя
  //mainBoard->calibrate();

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