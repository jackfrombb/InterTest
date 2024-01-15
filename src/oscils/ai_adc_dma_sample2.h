#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define TIMES 1024 // Количество преобразований в одном кадре
#define SAMPLE_FREQUENCY 80000 // Частота дискретизации в Гц
#define GET_UNIT(x) ((x>>3) & 0x1) // Макрос для получения номера модуля АЦП по номеру канала
#define ADC_RESULT_BYTE 2 // Количество байтов в одном результате преобразования
#define ADC_CONV_LIMIT_EN 1 // Флаг для ограничения количества преобразований
#define ADC_CONV_MODE ADC_CONV_SINGLE_UNIT_1 // Режим преобразования (только АЦП1)
#define ADC_OUTPUT_TYPE ADC_DIGI_OUTPUT_FORMAT_TYPE1 // Формат вывода (только данные)

static uint16_t adc1_chan_mask = BIT(0); // Маска каналов АЦП1
static uint16_t adc2_chan_mask = 0; // Маска каналов АЦП2
static adc1_channel_t channel[1] = { ADC1_CHANNEL_0 }; // Массив каналов АЦП
static const char *TAG = "ADC DMA"; // Тег для логирования

// Функция для инициализации драйвера ADC continuous mode
static void continuous_adc_init(uint16_t adc1_chan_mask, uint16_t adc2_chan_mask, adc1_channel_t *channel, uint8_t channel_num)
{
    // Структура для конфигурации драйвера
    adc_digi_init_config_t adc_dma_config = {
        .max_store_buf_size = 1024, // Максимальный размер пула в байтах
        .conv_num_each_intr = TIMES, // Количество преобразований в одном прерывании
        .adc1_chan_mask = adc1_chan_mask, // Маска каналов АЦП1
        .adc2_chan_mask = adc2_chan_mask, // Маска каналов АЦП2
    };
    // Инициализация драйвера
    ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

    // Структура для конфигурации АЦП
    adc_digi_configuration_t dig_cfg = {
        .conv_limit_en = ADC_CONV_LIMIT_EN, // Флаг для ограничения количества преобразований
        .conv_limit_num = 250, // Максимальное количество преобразований
        .sample_freq_hz = SAMPLE_FREQUENCY, // Частота дискретизации в Гц
        .conv_mode = ADC_CONV_MODE, // Режим преобразования
        .format = ADC_OUTPUT_TYPE, // Формат вывода
    };

    // Массив для шаблона преобразования
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};

    // Задаем количество каналов в шаблоне
    dig_cfg.pattern_num = channel_num;

    // Заполняем шаблон преобразования
    for (int i = 0; i < channel_num; i++) {
        uint8_t unit = GET_UNIT(channel[i]); // Получаем номер модуля АЦП
        uint8_t ch = channel[i] & 0x7; // Получаем номер канала АЦП

        adc_pattern[i].atten = ADC_ATTEN_DB_11; // Задаем аттенюацию
        adc_pattern[i].channel = ch; // Задаем канал
        adc_pattern[i].unit = unit; // Задаем модуль
        adc_pattern[i].bit_width = 12; // Задаем разрядность

        // Выводим информацию о шаблоне в лог
        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%d", i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%d", i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%d", i, adc_pattern[i].unit);
    }

    // Задаем шаблон преобразования в структуре конфигурации АЦП
    dig_cfg.adc_pattern = adc_pattern;

    // Настраиваем АЦП с помощью структуры конфигурации
    ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
}

// Функция для запуска процесса преобразования
esp_err_t adc_init()
{
    esp_err_t error_code;

    // Инициализируем драйвер ADC continuous mode
    continuous_adc_init(adc1_chan_mask, adc2_chan_mask, channel, sizeof(channel) / sizeof(adc_channel_t));

    // Настраиваем разрядность АЦП1
    error_code = adc1_config_width(ADC_WIDTH_BIT_12);
    if (error_code != ESP_OK) {
        return error_code;
    }

// Запускаем процесс преобразования
    adc_digi_start();

    return ESP_OK;
}

// Функция для чтения результатов преобразования
void adc_read()
{
    // Буфер для хранения результатов преобразования
    uint8_t *data = (uint8_t *)malloc(TIMES * ADC_RESULT_BYTE);
    uint32_t readed = 0;

    // Читаем результаты преобразования из пула
    int length = adc_digi_read_bytes(data, TIMES * ADC_RESULT_BYTE, &readed, ADC_MAX_DELAY);

    // Выводим количество прочитанных байтов в лог
    ESP_LOGI(TAG, "read %d Bytes", length);

    // Освобождаем буфер
    free(data);
}

void app_main(void)
{
    // Запускаем процесс преобразования
    adc_init();

    // Читаем результаты преобразования
    adc_read();

    // Останавливаем процесс преобразования
    adc_digi_stop();
}
