#include <Arduino.h>
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Количество каналов АЦП
#define ADC_CHANNEL_NUM 2
// Размер буфера для хранения результатов АЦП
#define ADC_BUFFER_SIZE 1024
// Количество преобразований АЦП в одном цикле
#define ADC_CONVERSIONS 256
// Частота преобразований АЦП в Гц
#define ADC_FREQUENCY 100000
// Максимальный размер пула памяти для DMA в байтах
#define ADC_POOL_SIZE 4096
// Размер одного результата преобразования АЦП в байтах
#define ADC_RESULT_SIZE 2
// Массив каналов АЦП
static adc_channel_t adc_channels[ADC_CHANNEL_NUM] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
// Маска каналов АЦП
static uint16_t adc_channel_mask = BIT(6) | BIT(7);
// Буфер для хранения результатов АЦП
static uint8_t adc_buffer[ADC_BUFFER_SIZE];
// // Указатель на дескриптор АЦП
// static adc_digi_handle_t adc_handle = NULL;
// Тег для логов
static const char *TAG = "ADC DMA";

// Функция инициализации АЦП
void adc_init()
{
    // Настраиваем параметры АЦП
    adc_digi_init_config_t adc_config = {
        .max_store_buf_size = ADC_POOL_SIZE,   // максимальный размер пула памяти для DMA
        .conv_num_each_intr = ADC_CONVERSIONS, // количество преобразований в одном прерывании
        .adc1_chan_mask = adc_channel_mask,    // маска каналов АЦП
        .adc2_chan_mask = 0,                   // не используем АЦП2
    };
    // Создаем дескриптор АЦП
    adc_digi_initialize(&adc_config);

    // Настраиваем параметры преобразований
    adc_digi_configuration_t dig_config = {
        .conv_limit_en = 1,                     // включаем ограничение количества преобразований
        .conv_limit_num = ADC_CONVERSIONS,      // устанавливаем лимит преобразований
        .sample_freq_hz = ADC_FREQUENCY,        // устанавливаем частоту преобразований
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,    // используем только АЦП1
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1, // используем формат вывода типа 1
    };
    // Настраиваем шаблон преобразований
    adc_digi_pattern_config_t adc_pattern[ADC_CHANNEL_NUM] = {0};
    dig_config.pattern_num = ADC_CHANNEL_NUM; // устанавливаем количество каналов в шаблоне
    for (int i = 0; i < ADC_CHANNEL_NUM; i++)
    {
        adc_pattern[i].atten = ADC_ATTEN_DB_11;         // устанавливаем коэффициент ослабления 11 дБ
        adc_pattern[i].channel = adc_channels[i] & 0x7; // устанавливаем номер канала
        adc_pattern[i].unit = ADC_UNIT_1;               // устанавливаем номер АЦП
        adc_pattern[i].bit_width = ADC_WIDTH_BIT_12;    // устанавливаем разрешение 12 бит
    }
    dig_config.adc_pattern = adc_pattern; // присваиваем шаблон преобразований
                                          // Применяем настройки преобразований
    adc_digi_controller_configure(&dig_config);
}

// Функция чтения результатов АЦП
void adc_read()
{
    // Запускаем преобразования АЦП
    adc_digi_start();
    // Ждем, пока пул памяти заполнится
    vTaskDelay(pdMS_TO_TICKS(1000));
    // Останавливаем преобразования АЦП
    adc_digi_stop();
    // Читаем данные из пула памяти в буфер
    size_t read_len = 0;
    adc_digi_read_bytes(adc_buffer, ADC_BUFFER_SIZE, &read_len, ADC_MAX_DELAY);

    for (int i = 0; i < read_len; i += ADC_RESULT_SIZE)
    {
        // Получаем значение АЦП из буфера
        uint16_t adc_value = (adc_buffer[i + 1] << 8) | adc_buffer[i];
        // Переводим значение АЦП в вольты
        float voltage = adc_value * 3.3 / 4096.0;
    }
}