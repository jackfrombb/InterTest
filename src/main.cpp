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


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <U8g2lib.h>

#ifdef BUZZER
#include "buzzer.h"
#endif


#ifdef KEYPAD
#include "keypad.h"
#endif

#include <driver/adc.h>

// Переменная дисплея
U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/CLK, /* data=*/DIN, /* cs=*/CE, /* dc=*/DC, /* reset=*/RST);

// Сохраняем параметры дисплея
const int displayHeight = u8g2.getDisplayHeight();
const int displayWidth = u8g2.getDisplayWidth();

//Хранение характеристик ADC
esp_adc_cal_characteristics_t *adc_chars; 

uint32_t readAnalogVal() {
  return adc1_get_raw(ADC1_CHANNEL_0);
}
Oscilloscope oscil = Oscilloscope(&readAnalogVal, 450);

int settingsVal = 0;      // 0 - Частота опроса, 1 - частота кадров, 2 - частота шима

int sectionsCountH = 3;
int sectionHeight = displayHeight / sectionsCountH;
const float maxMeasureValue = 3.2;
ulong framesForMenuTitleTimer = 0;

// Частота генерации
int pwmF = 1000;

#ifdef ENCODER
#include "encoder.h"
#endif

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

#ifndef EXCLUDE_GIVER_
//Пока пришлось вернуть, чуть позже переделаю
void encEvent()
{
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
    Serial.println(oscil.getTimer().playPause() ? "Pause - Oscil" : "Play - Oscil");
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
      auto alarm_value = oscil.getTimer().getTickTime() + (enc.dir() * steep);
      oscil.getTimer().setNewTickTime(alarm_value);
      Serial.println("Frq accur: " + String(alarm_value));
    }
    else if (settingsVal == 1)
    {
      const int steep = 100;
      auto alarm_value = oscil.getTimer().getTickTime() + (enc.dir() * steep);
      oscil.getTimer().setNewTickTime(alarm_value);
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
}
#endif

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
  u8g2.print(oscil.getInterruptTime() / 1000.0);
}

bool interfaceDrawInProcess = false; // Флаг начала прорисовки интерфейса

// Отрисовка в режиме осцилографа
void drawOscilograf(int32_t buf[])
{
  u8g2.firstPage();
  drawBack();
  drawValues(buf);
  u8g2.nextPage();
}

//int dropFps = 0;

bool IRAM_ATTR drawInterrupt(void *args)
{
  // Если интерфейс всё еще в прорисовке то пропускаем тик и засчитываем пропущенный кадр

  if (oscil.isBufferReady())
  {
    // Если буфер готов то начинаем прорисовку
    interfaceDrawInProcess = true;
    drawOscilograf(oscil.getBuffer());
    interfaceDrawInProcess = false;
    oscil.readNext();
  }

  return false;
}

void setup()
{
  const float vRef = 1.1;

  u8g2.begin(); // Инициализируем дисплей
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_10x20_t_cyrillic); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)

  String hello = "Привет";
  point pHello = getDisplayCener(hello);
  u8g2.setCursor(pHello.x, pHello.y);
  u8g2.print(hello);

  u8g2.sendBuffer();

//Конфигурация и настройка АЦП(ADC)
  //Указываем разрядность, канал и аттенюацию (ADC_ATTEN_DB_11 должен уменьшать макс напряжение до 2.5v)
  #ifdef S2MINI
  adc1_config_width(ADC_WIDTH_BIT_13);
  adc1_config_channel_atten(ADC1_CHANNEL_8, ADC_ATTEN_DB_11);
  #endif

  #ifdef WROOM32
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  #endif

  //Сохраняем характеристики АЦП для последующих преобразований
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

  #ifdef S2MINI
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_13, vRef, adc_chars);
  #endif

  #ifdef WROOM32
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, vRef, adc_chars);
  #endif
//Конец настройки АЦП
  delay(300);

  // Подсветка дисплея
  ledcAttachPin(DISPLAY_LED_PIN, 2);
  ledcSetup(2, 100, 8);
  ledcWrite(2, 150);

  #ifdef ENCODER
  setup_encoder();
  #endif

  #ifdef KEYPAD
  setup_keypad();
  #endif

  #ifdef BUZZ
  setup_buzzer();
  #endif

  // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcSetup(2, pwmF, 8);
  ledcWrite(2, 254 / 2);

  Serial.begin(9600);
  delay(1000);

  oscil.init();
  #ifdef ENCODER
  encoderTimer.init();
  #endif
}

void loop()
{
  #ifdef ENCODER
  loop_encoder();
  #endif

  #ifdef KEYPAD
  loop_keypad();
  #endif

  // Если буфер готов то начинаем прорисовку
  if (oscil.isBufferReady())
  {
    drawOscilograf(oscil.getBuffer());
    oscil.readNext();
  }
}