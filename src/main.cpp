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
//#include "esp32-hal-ledc.h" библиотека ledc

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <U8g2lib.h>

#include "encoder.h"

#include <driver/adc.h>

//#define S2MINI
#define WROOM32

//// Nokia PCD8544 display
//#define RST 2                       // Pin1 (RST)  GPIO2
//#define CE 15                       // Pin2 (CE)  GPIO15
//#define DC 4                        // Pin3 (DC)  GPIO4
//#define DIN 17                      // Pin4 (Din)  GPIO17
//#define CLK 18                      // Pin5 (Clk)  GPIO18
//                                    // Pin6 (Vcc)  3.3V
//#define DISPLAY_LED_PIN GPIO_NUM_10 // Pin7 (BL)
                                    // Pin8 (GND)  GND

//// Энкодер
//#define ENC_VCC GPIO_NUM_38
//#define ENC_CLCK GPIO_NUM_37
//#define ENC_DT GPIO_NUM_39
////#define ENC_SW GPIO_NUM_40 // Кнопка
////#define BUFFER_LENGTH 84

// Переменная дисплея
U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/CLK, /* data=*/DIN, /* cs=*/CE, /* dc=*/DC, /* reset=*/RST);
// Сохраняем параметры дисплея
const int displayHeight = u8g2.getDisplayHeight();
const int displayWidth = u8g2.getDisplayWidth();

esp_adc_cal_characteristics_t *adc_chars;


bool IRAM_ATTR oscillTimerInterrupt(void *args);



hard_timer oscilTimer = hard_timer(oscillTimerInterrupt, TIMER_GROUP_0, TIMER_1, 4500, 2);


// Буфер измерений
int32_t buffer[BUFFER_LENGTH];

int bufferOffset = 0;     // Смещение дисплея в буфере
volatile int lastPos = 0; // Последня позиция в буфере
int settingsVal = 0;      // 0 - Частота опроса, 1 - частота кадров, 2 - частота шима

int sectionsCountH = 3;
int sectionHeight = displayHeight / sectionsCountH;
const float maxMeasureValue = 3.2;
ulong framesForMenuTitleTimer = 0;

// Частота генерации
int pwmF = 100000;

// Точка на дисплее
struct point
{
  int x;
  int y;
};

/// @brief Координаты для отображения надписи по центру дисплея
/// @param title Надпись
/// @return point с координатами
static point getDisplayCener(String title)
{
  int x = displayWidth / 2 - ((title.length() * u8g2.getMaxCharWidth()) / 4);
  int y = displayHeight / 2 + u8g2.getMaxCharHeight() / 4;

  return point{
      .x = x,
      .y = y,
  };
}

ulong oscillInterruptTime = 0;
float mV = 0.0;

/// @brief Отрисовать ориентиры и надписи
void drawBack()
{
  // Смена управления вращение инкодера
  if (millis() - framesForMenuTitleTimer < 1000)
  {
    String title = "ERR";
    switch (settingsVal)
    {
    case 0:
      title = "Опрос";
      break;

    case 1:
      title = "Зум";
      break;

    case 2:
      title = "ШИМ";
      break;

    default:
      title = "ERR";
      break;
    }

    u8g2.setFont(u8g2_font_8x13_t_cyrillic);
    point pos = getDisplayCener(title);
    u8g2.setCursor(pos.x, pos.y); // На середину
    u8g2.print(title);
  }

  // Отображение значения регулируемого энкодером
  u8g2.setCursor(20, displayHeight);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  switch (settingsVal)
  {
  case 0:
    uint64_t val;
    timer_get_alarm_value(TIMER_GROUP_0, TIMER_1, &val);
    u8g2.print(String(val));
    break;

  case 1:
    break;

  case 2:
    u8g2.print(String(pwmF));
    break;

  default:
    break;
  }

  // Отрисовка точек деления шкалы
  u8g2.setFont(u8g2_font_4x6_tr);
  for (int8_t v = 1; v <= sectionsCountH; ++v)
  {
    int tickY = displayHeight - ((displayHeight * v) / sectionsCountH);

    for (uint8_t x = 0; x < displayWidth-1; x += 8)
    {
      int titlePos = displayWidth - 8;
      
      if (x >= titlePos)
      {
        u8g2.setCursor(titlePos, tickY + 10);
        u8g2.print(v);
      }

      u8g2.drawPixel(x, tickY);
    }
  }
}

/// @brief Отрисовать график
/// @param duration время старта
void drawValues(int32_t buf[])
{
  // Преобразованный предел
  static const int maxMeasureValNormalized = maxMeasureValue * 1000;

  // Оцилограмма
  for (uint8_t x = 0; x <= displayWidth; x++)
  {
    int realVolt = esp_adc_cal_raw_to_voltage(buf[x], adc_chars);
    int next = x == displayWidth ? 0 : buf[x + 1];

    // // Высчитывание среднего значения
    // mV = midArifm2(realVolt / 1000, displayWidth); // expRunningAverage(realVolt / 1000);
    byte val = map(realVolt, 0, maxMeasureValNormalized, displayHeight - 1, 0);

    if (x == displayWidth - 1)
    {
      u8g2.drawPixel(x, val);
    }
    else
    {
      byte val2 = map(esp_adc_cal_raw_to_voltage(next, adc_chars), 0, maxMeasureValue * 1000, displayHeight - 1, 0);
      u8g2.drawLine(x, val, x + 1, val2);
    }
  }

  u8g2.setCursor(0, 12);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.print(oscillInterruptTime / 1000.0);
}

volatile bool oscillPause = false;
volatile int maxMeasure = BUFFER_LENGTH;

bool interfaceDrawInProcess = false; // Флаг начала прорисовки интерфейса
bool bufferReady = false;            // Флаг окончания заполнения буфера значениями
// Отрисовка в режиме осцилографа
void drawOscilograf(int32_t buf[])
{
  u8g2.firstPage();
  drawBack();
  drawValues(buf);
  u8g2.nextPage();
}

int missTick = 0;            // Подсчитываем пропущеные тики
int synchTick = 0;           // Пропускаем для синхронизауии записи в буффер

// прерывание для измерений
bool IRAM_ATTR oscillTimerInterrupt(void *args)
{
  static ulong prevInterTime = 0;      // Предыдущее время тика

  // Если буфер готов то пропускаем заполнение
  if (bufferReady)
  {
    missTick += 1;
    return false;
  }

  // Си
  if(missTick > 0){
    synchTick = missTick%BUFFER_LENGTH;
    missTick = 0;
  }

  if(synchTick > 0){
    synchTick -= 1;
    return false;
  }

  oscillInterruptTime = micros() - prevInterTime;

  // Измерение
  uint32_t reading = adc1_get_raw(ADC1_CHANNEL_2);
  buffer[lastPos] = reading;

  if (lastPos == BUFFER_LENGTH)
  {
    lastPos = 0;
    bufferReady = true;
  }
  else
  {
    lastPos += 1;
  }

  prevInterTime = micros();
  return false;
}


//int dropFps = 0;

bool IRAM_ATTR drawInterrupt(void *args)
{
  // Если интерфейс всё еще в прорисовке то пропускаем тик и засчитываем пропущенный кадр

  if (bufferReady)
  {
    // Если буфер готов то начинаем прорисовку
    interfaceDrawInProcess = true;
    drawOscilograf(buffer);
    interfaceDrawInProcess = false;
    bufferReady = false;
  }

  return false;
}

void setup()
{
  u8g2.begin(); // Инициализируем дисплей
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_10x20_t_cyrillic); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)

  String hello = "Привет";
  point pHello = getDisplayCener(hello);
  u8g2.setCursor(pHello.x, pHello.y);
  u8g2.print(hello);

  u8g2.sendBuffer();

  #ifdef S2MINI
  adc1_config_width(ADC_WIDTH_BIT_13);
  adc1_config_channel_atten(ADC1_CHANNEL_8, ADC_ATTEN_DB_11);
  #endif

  #ifdef WROOM32
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_11);
  #endif

  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

  #ifdef S2MINI
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_13, 1.1, adc_chars);
  #endif

  #ifdef WROOM32
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 1.1, adc_chars);
  #endif

  delay(300);

  // Подсветка дисплея
  ledcAttachPin(DISPLAY_LED_PIN, 2);
  ledcSetup(2, 100, 8);
  ledcWrite(2, 10);

  setup_encoder();

  // Настройка шим
  ledcAttachPin(GPIO_NUM_35, 3);
  ledcSetup(3, pwmF, 8);
  ledcWrite(3, 254 / 2);

  Serial.begin(9600);
  delay(1000);

  oscilTimer.init();
  encoderTimer.init();
}

void loop()
{
  
  loop_encoder();

  // Если буфер готов то начинаем прорисовку
  if (bufferReady)
  {
    drawOscilograf(buffer);
    bufferReady = false;
  }
}