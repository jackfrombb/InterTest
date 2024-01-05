// ver: 0_0_j (номер релиза_любой номер комита_автор)

#include <Arduino.h>

// Все настройки железа здесь
#include "configuration.h"

// Основной буфер.
#define BUFFER_LENGTH 168

// Вспомогательные методы общие
#include "helpers.h"
// Вспомогательные структуры дисплея
#include "display_structs.h"

// Логика осцилографа
#include "oscil.h"
//Логика вольтметра
#include "voltmeter.h"
// Логика тамера прерываний
#include "hard_timer.h"

// esp32 библиотеки для работы ADC
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/i2s.h"

// Пищалка
#ifdef BUZZ
#include "buzzer.h"
#endif

// Определение контроллера
#ifdef S2MINI
#include "board_s2mini.h"
#elif defined(WROOM32)
#include "board_wrom32.h"
#endif

// Определение дисплея
#ifdef NOKIA5110_
// Nokia PCD8544 display
#include "display_nokia_5110.h"
#elif defined(OLED128x32_)
// дисплей 0.96 OLED I2C
#include "display_128x32.h"
#endif

// Сохраняем параметры дисплея
const int displayHeight = u8g2.getHeight();
const int displayWidth = u8g2.getWidth();

bool interfaceDrawInProcess = false; // Флаг начала прорисовки интерфейса

// Хранение характеристик ADC
esp_adc_cal_characteristics_t *adc_chars;

OscilAdc oscil = OscilAdc(&mainBoard, 8402); // board_readAnalogVal - определяется в файле board_***.h
Voltmetr voltmetr = Voltmetr();

int settingsVal = 0;               // 0 - Частота опроса, 1 - частота кадров, 2 - частота шима
const float maxMeasureValue = 3.2; // Потолок по напряжению, если ниже 3.0 то ломается. Больше можно
ulong framesForMenuTitleTimer = 0; // Счетчик кадров для отображения названия меню, его увеличивает control, а отслеживает interface

// Частота генерации
int pwmF = 10000;

#ifdef ENCODER
#include "control_encoder.h"
#elif defined(KEYPAD)
#include "control_keypad.h"
#endif

#include "display_helper.h"

// Nokia PCD8544 display
#ifdef NOKIA5110_
#include "interface_wide.h"
// дисплей 0.96 OLED I2C
#elif defined(OLED128x32_)
#include "interface_wide.h"
#endif

void i2sInit()
{
  Serial.println("I2s init start");
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
      .sample_rate = 20,                            // The format of the signal using ADC_BUILT_IN
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = I2S_COMM_FORMAT_STAND_MSB,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8, // number of DMA buffers
      .dma_buf_len = 1024,   // number of samples (in bytes)
      .use_apll = false,   // no Audio PLL
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0};

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_0);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit,
                                                          ESP_ADC_CAL_VAL_DEFAULT_VREF, adc_chars);

  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    Serial.println("eFuse Vref");
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    Serial.println("Two Point");
  }
  else
  {
    Serial.println("Default");
  }

  Serial.println("I2s init ok");

  // SYSCON.saradc_ctrl2.sar1_inv = 1;     // SAR ADC samples are inverted by default
  // SYSCON.saradc_ctrl.sar1_patt_len = 0; // Use only the first entry of the pattern table
  delay(1000); // required for stability of ADC
  auto i2c_adc_err = i2s_adc_enable(I2S_NUM_0);

  if (i2c_adc_err == ESP_OK)
    Serial.println("I2s adc enabled");
  else if (i2c_adc_err == ESP_ERR_INVALID_ARG)
    Serial.println("I2s adc INVALIDE ARGS");
  else if (i2c_adc_err == ESP_ERR_INVALID_STATE)
    Serial.println("I2s adc INVALIDE STATE");

  delay(100); // required for stability of ADC
}

size_t bytes_read;
int32_t buffer[512];
unsigned long t_start = micros();
unsigned long fft_loop_cntr = 0;
bool bufferReady = false;

void i2sReadSignal(void *pvParameters)
{
  // while(1){
  // Serial.println("Read");
  // delay(1000);
  // }

  //(I2S port, destination adress, data size in bytes, bytes read counter, RTOS ticks to wait)
  while (1)
  {
      auto resultI2cRead = i2s_read(I2S_NUM_0, &buffer, 512, &bytes_read, 15);
      bufferReady = true;
      delay(1000);
  }

  // if (resultI2cRead == ESP_OK)
  //   Serial.println("I2C Read OK" + String());
  // else
  //   Serial.println("I2C Read ERROR" + String());
}

void setup()
{
  Serial.begin(115200);
  delay(300);
  
  Serial.println("Start to config: display");
  display_init();

  Serial.print(" , mainBoard");
  mainBoard.init();

  u8g2.setFont(u8g2_font_10x20_t_cyrillic); // Выставляем шрифт (шрифты жрут прорву памяти так что аккуратнее если меняете)
  String hello = "Привет";
  point_t pHello = getDisplayCener(hello, u8g2.getMaxCharWidth(), u8g2.getBufferTileHeight());
  u8g2.setCursor(pHello.x, pHello.y);
  u8g2.print(hello);
  u8g2.sendBuffer();

  delay(300);

  Serial.print(" , control");
  control_init();

#ifdef BUZZ

  Serial.print(" , buzzer");
  setup_buzzer();
#endif

  Serial.print(" , pwm");
  // Настройка шим - временный костыль для проверки АЦП, позже вынесем в отдельный класс генератора
  ledcSetup(2, pwmF, 8);
  ledcAttachPin(GPIO_NUM_4, 2);
  ledcWrite(2, 254/2);

  delay(300);
  
  Serial.print(" , oscil");
  oscil.init();
  
  Serial.print(" , voltmetr");
  voltmetr.setAdcChars(adc_chars);

//Прикрепить процесс к ядру
  // xTaskCreatePinnedToCore(
  //     i2sReadSignal,   // Function to implement the task
  //     "i2sReadSignal", // Name of the task
  //     1000,            // Stack size in bytes
  //     NULL,            // Task input parameter
  //     10,               // Priority of the task
  //     NULL,            // Task handle.
  //     0               // Core where the task should run
  // );
}

void loop()
{
  control_loop();

  // i2sReadSignal();
  //  Если буфер готов то начинаем прорисовку

  if (oscil.isBufferReady())
  {
    drawOscilograf(oscil.getBuffer());
    oscil.readNext();
    // .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    //   .sample_rate = 200000,                            // The format of the signal using ADC_BUILT_IN
    //   .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    //   .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    //   .communication_format = I2S_COMM_FORMAT_STAND_MSB,
    //   .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    //   .dma_buf_count = 8, // number of DMA buffers
    //   .dma_buf_len = 1024,   // number of samples (in bytes)
    //   .use_apll = false,   // no Audio PLL
    //   .tx_desc_auto_clear = false,
    //   .fixed_mclk = 0};
    //i2s_set_clk(I2S_NUM_0, i2s_get_clk(I2S_NUM_0) + 10, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  }
}