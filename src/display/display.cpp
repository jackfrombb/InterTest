/* **********************************************
Author: JackFromBB - jack@boringbar.ru / 
Placement from: https://github.com/jackfrombb/
The library for ESP32 under Arduino Environment

In cooperatiion with svdpfaf (svddevelop@gmail.com)

************************************************ */

#include "display.h"
#include "common.h"
#include <Arduino.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <U8g2lib.h>
#include "oscil.h"
#include <esp_adc_cal.h>

#include "encoder.h"

// Переменная дисплея
#ifdef DISPU8X8_
    U8G2 u8g2;
#else
    U8G2 u8g2;
#endif


// Сохраняем параметры дисплея
const int displayHeight = u8g2.getDisplayHeight();
const int displayWidth = u8g2.getDisplayWidth();
//Хранение характеристик ADC
esp_adc_cal_characteristics_t *adc_chars; 

bool interfaceDrawInProcess = false; // Флаг начала прорисовки интерфейса

Oscilloscope oscil = Oscilloscope(&readAnalogVal, 450);


int sectionsCountH = 3;
int sectionHeight = displayHeight / sectionsCountH;
const float maxMeasureValue = 3.2;

ulong framesForMenuTitleTimer = 0;
int settingsVal = 0;      // 0 - Частота опроса, 1 - частота кадров, 2 - частота шима

// Точка на дисплее
typedef struct 
{
  int x;
  int y;
} point_t;


uint32_t readAnalogVal() {
  return adc1_get_raw(ADC1_CHANNEL_0);
}

/// @brief Координаты для отображения надписи по центру дисплея
/// @param title Надпись
/// @return point с координатами
static point_t getDisplayCener(String title)
{
  int x = displayWidth / 2 - ((title.length() * u8g2.getMaxCharWidth()) / 4);
  int y = displayHeight / 2 + u8g2.getMaxCharHeight() / 4;

  return point_t{
      .x = x,
      .y = y,
  };
}

// Отрисовка в режиме осцилографа
void drawOscilograf(int32_t buf[])
{
  u8g2.firstPage();
  drawBack();
  drawValues(buf);
  u8g2.nextPage();
}




/// @brief Отрисовать ориентиры и надписи
void drawBack()
{
  // Смена управления вращение инкодера
  if (millis() - framesForMenuTitleTimer < 1000)
  {
    String title = "ERR";
    switch ( settingsVal )
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
    point_t pos = getDisplayCener(title);
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
void drawValues(int32_t a_buf[])
{
  // Преобразованный предел
  static const int maxMeasureValNormalized = maxMeasureValue * 1000;

  // Оцилограмма
  for (uint8_t x = 0; x <= displayWidth; x++)
  {
    int realVolt = esp_adc_cal_raw_to_voltage(a_buf[x], adc_chars);
    int next = x == displayWidth ? 0 : a_buf[x + 1];

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


void setup_display(){

    const float vRef = 1.1;

    

    #if DISPLAYTYPE_ == OLED128x32
        //pinMode( DSP_SCK_, OUTPUT );
        //pinMode( DSP_SDA_, OUTPUT );
        //U8G2_SH1106_128X32_VISIONOX_F_HW_I2C loc_dsp(U8G2_R0, U8X8_PIN_NONE, DSP_SCK_, DSP_SDA_);
        U8G2_SSD1306_128X64_NONAME_F_SW_I2C loc_dsp(U8G2_R0, DSP_SCK_, DSP_SDA_, U8X8_PIN_NONE );
        u8g2 = loc_dsp;
    #else
        U8G2_PCD8544_84X48_F_4W_SW_SPI loc_dsp(U8G2_R0, /* clock=*/CLK, /* data=*/DIN, /* cs=*/CE, /* dc=*/DC, /* reset=*/RST);
        u8g2 = loc_dsp;
    #endif


  u8g2.begin(); // Инициализируем дисплей
  #if defined( DISPU8X8_ )
    #define SSD1306_SETPRECHARGE 0xD9
    #define SSD1306_SETCONTRAST 0x81

    //u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 180);
    //half u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 90);
    //u8g2.sendF("caaaaaaaaaaaaaaaa", 0xb8, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 45);
    u8g2.setContrast(255);
    u8g2.setPowerSave(0);

  #endif
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_10x20_t_cyrillic); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)

  String hello = "Привет";
  point_t pHello = getDisplayCener(hello);
  u8g2.setCursor(pHello.x, pHello.y);
  u8g2.print(hello);

  u8g2.sendBuffer();

  // Подсветка дисплея
  ledcAttachPin(DISPLAY_LED_PIN, 2);
  ledcSetup(2, 100, 8);
  ledcWrite(2, 150);

 
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
 
  oscil.init();
}

void lopp_display(){

 // Если буфер готов то начинаем прорисовку
  if (oscil.isBufferReady())
  {
    drawOscilograf(oscil.getBuffer());
    oscil.readNext();
  }
}
