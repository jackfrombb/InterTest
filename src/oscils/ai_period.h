// Подключаем библиотеки
#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <arduinoFFT.h> // Подключаем библиотеку arduinoFFT

// Определяем константы
#define ADC_CHANNEL ADC1_CHANNEL_0 // GPIO 36
#define ADC_WIDTH ADC_WIDTH_BIT_12 // 12 бит
#define ADC_ATTEN ADC_ATTEN_DB_11 // Диапазон 0-3.6 В
#define ADC_UNIT ADC_UNIT_1 // ADC1
#define SAMPLE_RATE 1000 // Герц
#define SAMPLE_SIZE 1024 // Количество отсчетов

// Создаем массивы для хранения данных
int16_t data[SAMPLE_SIZE]; // Массив для данных с ADC
double vReal[SAMPLE_SIZE]; // Массив для действительной части БПФ
double vImag[SAMPLE_SIZE]; // Массив для мнимой части БПФ

// Создаем объект для БПФ
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLE_SIZE, SAMPLE_RATE);

// Создаем переменную для хранения калибровки
esp_adc_cal_characteristics_t *adc_chars;

// Функция для инициализации ADC
void adc_init() {
  // Настраиваем ADC
  adc1_config_width(ADC_WIDTH);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);

  // Выделяем память для калибровки
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

  // Калибруем ADC
  esp_adc_cal_characterize(ADC_UNIT, ADC_ATTEN, ADC_WIDTH, 1100, adc_chars);
}

// Функция для чтения данных с ADC
void adc_read() {
  // Запускаем таймер
  uint32_t start = millis();

  // Читаем данные с ADC в цикле
  for (int i = 0; i < SAMPLE_SIZE; i++) {
    // Ждем следующего отсчета
    while (millis() - start < i * 1000 / SAMPLE_RATE);

    // Читаем значение с ADC
    data[i] = adc1_get_raw(ADC_CHANNEL);

    // Преобразуем значение в вольты
    vReal[i] = esp_adc_cal_raw_to_voltage(data[i], adc_chars) / 1000.0;
    vImag[i] = 0; // Мнимая часть равна нулю
  }
}

// Функция для вычисления частоты сигнала с помощью БПФ
float adc_freq_fft() {
    
  // Выполняем БПФ
  FFT.DCRemoval(); // Удаляем постоянную составляющую
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD); // Применяем оконную функцию
  FFT.Compute(FFT_FORWARD); // Вычисляем БПФ
  FFT.ComplexToMagnitude(); // Получаем модуль комплексных чисел

  // Ищем максимальное значение в спектре
  double maxVal = 0;
  int maxIndex = 0;
  for (int i = 2; i < SAMPLE_SIZE / 2; i++) { // Пропускаем первые два значения (постоянная и нулевая частоты)
    if (vReal[i] > maxVal) {
      maxVal = vReal[i];
      maxIndex = i;
    }
  }

  // Возвращаем частоту сигнала

  return FFT.Frequency(maxIndex);
}

// Функция setup
void setup() {
  // Настраиваем серийный порт
  Serial.begin(115200);

  // Инициализируем ADC
  adc_init();
}

// Функция loop
void loop() {
  // Читаем данные с ADC
  adc_read();

  // Вычисляем частоту сигнала с помощью БПФ
  float freq = adc_freq_fft();

  // Выводим результат на серийный порт
  Serial.print("Frequency: ");
  Serial.print(freq);
  Serial.println(" Hz");

  // Ждем 1 секунду
  delay(1000);
}