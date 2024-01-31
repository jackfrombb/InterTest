/*
Пример генерации кода по запросу:
напиши код для вывода буфера значений в dac, через  i2s драйвер, для генерации меандра, для esp32 и версии espidf 4.4.6

С попытками испраить и подогнать под что то близкое к реальному
*/
#include "driver/i2s.h"
#include "driver/dac.h"

#define SAMPLE_RATE 44100 // частота дискретизации в Гц
#define SAMPLE_BITS i2s_bits_per_sample_t::I2S_BITS_PER_SAMPLE_16BIT    // разрядность сэмпла в битах
#define CHANNELS 2        // количество каналов
#define I2S_NUM I2S_NUM_0 // номер I2S порта
#define DMA_BUF_COUNT 2   // количество DMA буферов
#define DMA_BUF_LEN 1024  // длина DMA буфера в сэмплах
#define MEANDER_FREQ 1000 // частота меандра в Гц
#define MEANDER_AMP 127   // амплитуда меандра в диапазоне от 0 до 127

// функция для инициализации I2S драйвера
void i2s_init()
{
    // настройка параметров I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN), // режим работы I2S: мастер, передача, встроенный DAC
        .sample_rate = SAMPLE_RATE,                                    // частота дискретизации
        .bits_per_sample = SAMPLE_BITS,                                // разрядность сэмпла
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                  // формат каналов: правый-левый
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,               // формат коммуникации: I2S, старший бит первый
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,                      // флаги прерываний
        .dma_buf_count = DMA_BUF_COUNT,                                // количество DMA буферов
        .dma_buf_len = DMA_BUF_LEN,                                    // длина DMA буфера
        .use_apll = false                                              // использование APLL
    };

    // настройка пинов I2S
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,   // пин BCK не используется
        .ws_io_num = I2S_PIN_NO_CHANGE,    // пин WS не используется
        .data_out_num = I2S_PIN_NO_CHANGE, // пин DATA_OUT не используется
        .data_in_num = I2S_PIN_NO_CHANGE   // пин DATA_IN не используется
    };

    // установка I2S драйвера
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);

    // установка пинов I2S
    i2s_set_pin(I2S_NUM, &pin_config);

    // включение каналов DAC
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
}

// функция для генерации меандра
void generate_meander(uint8_t *data, size_t len)
{
    // вычисление периода меандра в сэмплах
    int period = SAMPLE_RATE / MEANDER_FREQ;

    // вычисление половины периода меандра в сэмплах
    int half_period = period / 2;

    // вычисление значения сэмпла для верхнего уровня меандра
    uint8_t high = 0x80 + MEANDER_AMP;

    // вычисление значения сэмпла для нижнего уровня меандра
    uint8_t low = 0x80 - MEANDER_AMP;

    // переменная для хранения текущего значения сэмпла
    uint8_t value = high;

    // переменная для хранения текущего счётчика сэмплов
    int counter = 0;

    // цикл по всем сэмплам в буфере
    for (int i = 0; i < len; i++)
    {
        // запись значения сэмпла в буфер
        data[i] = value;

        // увеличение счётчика сэмплов
        counter++;

        // если счётчик достиг половины периода
        if (counter == half_period)
        {
            // смена значения сэмпла на противоположное
            value = (value == high) ? low : high;

            // обнуление счётчика
            counter = 0;
        }
    }
}

// функция для вывода буфера значений в DAC через I2S драйвер
void i2s_write_dac(uint8_t *data, size_t len)
{
    // переменная для хранения количества записанных байт
    size_t bytes_written = 0;

    // запись буфера в I2S
    i2s_write(I2S_NUM, data, len, &bytes_written, portMAX_DELAY);
}

// главная функция
void app_main()
{
    // инициализация I2S драйвера
    i2s_init();

    // создание буфера для хранения сэмплов
    uint8_t *buffer = (uint8_t *) malloc(DMA_BUF_LEN);

    // бесконечный цикл
    while (true)
    {
        // генерация меандра в буфер
        generate_meander(buffer, DMA_BUF_LEN);

        // вывод буфера в DAC через I2S драйвер
        i2s_write_dac(buffer, DMA_BUF_LEN);
    }
}
