#include <Arduino.h>

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

/// @brief Структура для передачи данных измерений в интерфейс
// Пока не придумал где её хранить, что бы избежать "взаимных зависимостей", потому лежит здесь
typedef struct
{
  uint16_t *buffer;    // Буфер значений (заполняет ацп в логике осцилографа)
  uint16_t bufferSize; // Размер буфера
  uint16_t middle;     // Среднее значение высчитывает вольтметр
  uint16_t max;        // Максимальное
  uint16_t min;        // Минимальное
  int16_t bias;        // Смещение для синхронизации периодических сигналов
  uint16_t readedSize; // Размер считанного в ацп
} adc_measures_t;

#include "module_virtual.h" // Абстракция для того что необходимо инициализировать перед использованием (перестал применять, но не удалил)

#include "controls/control_virtual.h" // Абстракция управления

#include "interface/ellements/ellements_structs.h" // Вспомогательные структуры граф элементов
#include "interface/ellements/ellement_virtual.h"  // Абстракция граф элементов

#include "displays/display_virtual.h"           // Абстракция дисплея.  Cоздаёт двигатель отрисовки (InterfaceEngine)
#include "interface/ellements/ellements_list.h"    // Список элементов
#include "interface/engines/interface_engine.h" // Абстракция двигателя отрисовки

#include "controllers/adc_virtual.h" //Абстракция контроллера adc
#include "board_virtual.h"           // Абстракция главной платы/контроллера
#include "oscils/oscil_virtual.h"    // Абстракция над логикой осциографа
#include "oscils/oscil_adc_dma.h"    // Логикой осциографа continue

#include "hard_timer.h"         // Логика тамера прерываний
#include "oscils/oscils_list.h" // Логика осцилографа
#include "voltmeter.h"          // Логика вольтметра
#include "signal_generator.h"   // Логика генератора сигналов

#include "interface/pages/views/page_view.h"      // Абстракция представления страницы граф. интерфейся
#include "interface/pages/page_virtual.h"         // Абстракция над контроллера странички
#include "interface/ellements/ellement_virtual.h" // Абстракция элемента графического дизайна

// Перенесено в класс дисплея, что бы упростить переключение железа в конфигурации
// // Определяем UI
// #ifdef WIDE_UI
// #include "interface/pages/views/wide_screen/wide_views_list.h" // Обычные дисплеи типа 128:64, 84:48
// #elif defined(SLIM_UI)
// #include "interface/pages/views/slim_screen/slim_views_list.h" // Узкие дисплеи типа 128:32
// #endif

#ifdef BUZZ
#include "buzzer.h" // Пищалка
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
#include "displays/display_128x64.h"
DisplayVirtual *display = new Display128x64_U8g2();
#elif defined(ST7735_TFT_128x160_1_8)
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
#elif defined(WROOM32)
#include "boards/esp32_wroom32.h"
MainBoard *mainBoard = new Esp32Wroom32(display, control);
#endif

InterfaceController interfaceController(mainBoard);

// Частота генерации
int pwmF = 100000;
// USBCDC usbSerial;

// #include "ai_samples/dac_i2s_4_4_6__timer.h"

void setup()
{
  Serial.begin(115200);
  
  // while (!Serial) // Эта строка не дает загрузится устройству пока не запустится Serial костыль для отладки s2mini
  // {
  //   vTaskDelay(1000); // Что бы не будить псов
  // };
  // Serial.println("SERIAL: Setup complete");

  // logi::settings(true, &usbSerial);

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

  // Инициализация синглтона хранения настроек/состояний
  AppData::begin();
  
  // Инициализация синглтона генератора
  SignalGenerator::init(mainBoard->getPwmPin());

#ifdef BUZZ
  Serial.println("Buzzer");
  setup_buzzer();
#endif

  *progress = 0.6;
  delay(300);

  // Временный костыль для проверки АЦП
  SignalGenerator::get()->startMeandrLedc(pwmF, 0.5);
  // sigGen.startMenadrDac(pwmF, .5);
  // sigGen.startWaveDac(pwmF);
  // создание буфера для хранения сэмплов
  // buffer = (uint8_t *)malloc(DMA_BUF_LEN);
  // // инициализация I2S драйвера
  // i2s_init();
  // // инициализация таймера
  // timer_init();

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