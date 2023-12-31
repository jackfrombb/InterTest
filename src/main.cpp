// #include <Arduino.h>
//  USE_SSD1306 // Use I2C OLED screen on SSD1306 chipset
#include <helpers.h>
#include <oscil.h>
#include <hard_timer.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"
//#include "driver/timer.h"
// #include "esp_timer.h"
//  #include "esp_adc_cal.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#include <U8g2lib.h>
#include <EncButton.h>

// Nokia PCD8544 display
#define RST 2                       // Pin1 (RST)  GPIO2
#define CE 15                       // Pin2 (CE)  GPIO15
#define DC 4                        // Pin3 (DC)  GPIO4
#define DIN 17                      // Pin4 (Din)  GPIO17
#define CLK 18                      // Pin5 (Clk)  GPIO18
                                    // Pin6 (Vcc)  3.3V
#define DISPLAY_LED_PIN GPIO_NUM_10 // Pin7 (BL)
                                    // Pin8 (GND)  GND

// Энкодер
#define ENC_VCC GPIO_NUM_38
#define ENC_CLCK GPIO_NUM_37
#define ENC_DT GPIO_NUM_39
#define ENC_SW GPIO_NUM_40

#define BUFFER_LENGTH 84

// Переменная дисплея
U8G2_PCD8544_84X48_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/CLK, /* data=*/DIN, /* cs=*/CE, /* dc=*/DC, /* reset=*/RST);
// Сохраняем параметры дисплея
const int displayHeight = u8g2.getDisplayHeight();
const int displayWidth = u8g2.getDisplayWidth();

esp_adc_cal_characteristics_t *adc_chars;

// Энкодер
EncButton enc(ENC_DT, ENC_CLCK, ENC_SW);

bool IRAM_ATTR oscillTimerInterrupt(void *args);
bool IRAM_ATTR encTick(void *args);

hard_timer oscilTimer = hard_timer(oscillTimerInterrupt, TIMER_GROUP_0, TIMER_1, 4500, 2);
hard_timer encoderTimer = hard_timer(encTick, TIMER_GROUP_1, TIMER_0, 600, 80);

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

    for (uint8_t x = 0; x < displayWidth; x += 8)
    {
      int displayVSect = displayWidth - (displayWidth + 1);

      if (x >= displayVSect)
      {
        int sectionValue = v;

        u8g2.setCursor(displayVSect, tickY + 10);
        u8g2.print(sectionValue);
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
void drawOscilograf(int buf[])
{
  u8g2.firstPage();
  drawBack();
  drawValues(buf);
  u8g2.nextPage();
}

int missTick = 0;            // Подсчитываем пропущеные тики
int synchTick = 0;           // Пропускаем для синхронизауии записи в буффер

// прерыване для измерений
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
  uint32_t reading = adc1_get_raw(ADC1_CHANNEL_8);
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

bool IRAM_ATTR encTick(void *args)
{
  enc.tickISR();
  return false;
}

float vRef = 1.1;
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
    Serial.println(oscilTimer.playPause() ? "Pause - Oscil" : "Play - Oscil");
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
      auto alarm_value = oscilTimer.getTickTime() + (enc.dir() * steep);
      oscilTimer.setNewTickTime(alarm_value);
      Serial.println("Frq accur: " + String(alarm_value));
    }
    else if (settingsVal == 1)
    {
      const int steep = 100;
      auto alarm_value = oscilTimer.getTickTime() + (enc.dir() * steep);
      oscilTimer.setNewTickTime(alarm_value);
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

int dropFps = 0;
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
  // pinMode(12, INPUT_PULLUP);  // 1 кнопка от юсб разьема
  //  pinMode(7, INPUT_PULLUP);  // 2 кнопка от юсб разьема
  // pinMode(8, INPUT_PULLUP);   // 3 кнопка от юсб разьема
  // pinMode(5, INPUT_PULLUP);   // пин а5 вольтметр и осцилограф
  // pinMode(4, INPUT);          // пин 4 ввод данных частотомер
  //  pinMode(9, OUTPUT);        // пин 9 выход на генератор
  // pinMode(13, OUTPUT);        // пин 13 генератор для проверки осцила и частотомера

  u8g2.begin(); // Инициализируем дисплей
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_10x20_t_cyrillic); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)
  u8g2.sendBuffer();                        // Отсылаем данные на дисплей
                                            // u8g2.setDisplayRotation(U8G2_R2);   // Поворачиваем дисплей на 180

  String hello = "Привет";
  point pHello = getDisplayCener(hello);
  u8g2.setCursor(pHello.x, pHello.y);
  u8g2.print(hello);

  u8g2.sendBuffer();

  adc1_config_width(ADC_WIDTH_BIT_13);
  adc1_config_channel_atten(ADC1_CHANNEL_8, ADC_ATTEN_DB_11);

  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_13, 1.1, adc_chars);

  delay(300);

  // Подсветка дисплея
  ledcAttachPin(DISPLAY_LED_PIN, 2);
  ledcSetup(2, 100, 8);
  ledcWrite(2, 10);

  // Запитываем энкодер и подключаем к нему событие
  pinMode(ENC_VCC, OUTPUT);
  digitalWrite(ENC_VCC, 1);
  enc.attach(encEvent);

  // Настройка шим
  ledcAttachPin(GPIO_NUM_35, 3);
  ledcSetup(3, pwmF, 8);
  ledcWrite(3, 254 / 2);

  // const timer_config_t config_accur = {
  //     .alarm_en = TIMER_ALARM_EN,
  //     .counter_en = TIMER_PAUSE,
  //     .intr_type = TIMER_INTR_LEVEL,
  //     .counter_dir = TIMER_COUNT_UP,
  //     .auto_reload = TIMER_AUTORELOAD_EN,
  //     .divider = 2,
  // };

  // const timer_config_t config_slow = {
  //     .alarm_en = TIMER_ALARM_EN,
  //     .counter_en = TIMER_PAUSE,
  //     .intr_type = TIMER_INTR_LEVEL,
  //     .counter_dir = TIMER_COUNT_UP,
  //     .auto_reload = TIMER_AUTORELOAD_EN,
  //     .divider = 80,
  // };

  // // Прерывние измерений
  // timer_init(TIMER_GROUP_0, TIMER_1, &config_accur);
  // timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0);
  // timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, 4500);
  // timer_isr_callback_add(TIMER_GROUP_0, TIMER_1, oscillTimerInterrupt, NULL, 0);
  // timer_enable_intr(TIMER_GROUP_0, TIMER_1);
  // timer_start(TIMER_GROUP_0, TIMER_1);

  // Прерывание энкодера
  // timer_init(TIMER_GROUP_1, TIMER_0, &config_slow);
  // timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0);
  // timer_set_alarm_value(TIMER_GROUP_1, TIMER_0, 600);
  // timer_isr_callback_add(TIMER_GROUP_1, TIMER_0, encTick, NULL, 0);
  // timer_enable_intr(TIMER_GROUP_1, TIMER_0);
  // timer_start(TIMER_GROUP_1, TIMER_0);
  //encoderTimer = hard_timer(encTick, TIMER_GROUP_1, TIMER_0, 600, 80);

  Serial.begin();
  delay(1000);

  oscilTimer.init();
  encoderTimer.init();
}

void loop()
{
  enc.tick();

  // Если буфер готов то начинаем прорисовку
  if (bufferReady)
  {
    drawOscilograf(buffer);
    bufferReady = false;
  }
}