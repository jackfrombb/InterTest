#include "driver/i2s.h"
#include "driver/dac.h"
#include "driver/timer.h"
#include "logi.h"

#define SAMPLE_RATE 44100                                            // частота дискретизации в Гц
#define SAMPLE_BITS i2s_bits_per_sample_t::I2S_BITS_PER_SAMPLE_16BIT // разрядность сэмпла в битах
#define CHANNELS 1                                                   // количество каналов
#define I2S_NUM i2s_port_t::I2S_NUM_0                                // номер I2S порта
#define DMA_BUF_COUNT 2                                              // количество DMA буферов
#define DMA_BUF_LEN 256                                             // длина DMA буфера в сэмплах
#define MEANDER_FREQ 1000                                            // частота меандра в Гц
#define MEANDER_AMP 127                                              // амплитуда меандра в диапазоне от 0 до 127
#define TIMER_GROUP TIMER_GROUP_0                                    // номер группы таймеров
#define TIMER_NUM TIMER_0                                            // номер таймера в группе
#define TIMER_DIVIDER 80                                             // делитель таймера
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)                 // масштаб таймера
#define TIMER_INTERVAL_SEC (1.0 / MEANDER_FREQ / 2.0)                // интервал таймера в секундах

// переменная для хранения указателя на буфер
uint8_t *buffer = NULL;

// переменная для хранения текущего значения сэмпла
uint8_t value = (uint8_t)(0x80 + MEANDER_AMP);

// функция для инициализации I2S драйвера
void i2s_init()
{
    // настройка параметров I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS, /* the DAC module will only take the 8bits from MSB */
        .channel_format = i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_LEN,
        .use_apll = false
        // .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN), // режим работы I2S: мастер, передача, встроенный DAC
        // .sample_rate = (uint32_t)SAMPLE_RATE,                                        // частота дискретизации
        // .bits_per_sample = SAMPLE_BITS,                                              // разрядность сэмпла
        // .channel_format = i2s_channel_fmt_t::I2S_CHANNEL_FMT_ONLY_RIGHT,             // формат каналов: правый-левый
        // .communication_format = i2s_comm_format_t::I2S_COMM_FORMAT_STAND_MSB,        // формат коммуникации: I2S, старший бит первый
        // .intr_alloc_flags = (int)ESP_INTR_FLAG_LEVEL1,                               // флаги прерываний
        // .dma_buf_count = (int)DMA_BUF_COUNT,                                         // количество DMA буферов
        // .dma_buf_len = (int)512,                                                     // длина DMA буфера
        // .use_apll = false,                                                           // использование APLL

    };

    // установка I2S драйвера
    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    // установка пинов I2S
    i2s_set_pin(I2S_NUM, NULL);
    // включение каналов DAC
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
    i2s_set_sample_rates(I2S_NUM, 22050); // set sample rates

    // dac_output_enable(dac_channel_t::DAC_CHANNEL_1);
    // dac_i2s_enable();
}

IRAM_ATTR void _workThread(void *pvParameters);

// функция для инициализации таймера
void timer_init()
{
    xTaskCreatePinnedToCore(
        _workThread,     // Function to implement the task
        "workThreadDac", // Name of the task
        1024,            // Stack size in bytes
        NULL,            // Task input parameter
        1100,            // Priority of the task
        NULL,            // Task handle.
        0                // Core where the task should run
    );
}

IRAM_ATTR void _workThread(void *pvParameters)
{
    while (true)
    {
        // смена значения сэмпла на противоположное
        value = (value == 0x80 + MEANDER_AMP) ? (uint8_t)(0x80 - MEANDER_AMP) : (uint8_t)(0x80 + MEANDER_AMP);

        // заполнение буфера текущим значением сэмпла
        for (int i = 0; i < DMA_BUF_LEN; i++)
        {
            buffer[i] = value;
        }
        uint32_t written = 0;
        
        i2s_write(I2S_NUM, buffer, DMA_BUF_LEN, &written, portMAX_DELAY);
        vTaskDelay(1);
    }
}

// функция для обработки прерывания таймера
void IRAM_ATTR timer_isr(void *arg)
{
    // сброс флага сигнализации таймера
    // timer_group_clr_intr_status_in_isr(TIMER_GROUP, TIMER_NUM);

    // // смена значения сэмпла на противоположное
    // value = (value == 0x80 + MEANDER_AMP) ? (uint8_t)(0x80 - MEANDER_AMP) : (uint8_t)(0x80 + MEANDER_AMP);

    // // заполнение буфера текущим значением сэмпла
    // for (int i = 0; i < DMA_BUF_LEN; i++)
    // {
    //     buffer[i] = value;
    // }

    // запись буфера в I2S
    // logi::err("I2S_DAC_SAMPLE", i2s_write(I2S_NUM, buffer, DMA_BUF_LEN, NULL, portMAX_DELAY));
    uint32_t written = 0;
    i2s_write(I2S_NUM, buffer, DMA_BUF_LEN, &written, portMAX_DELAY);

    // перезагрузка таймера
    // timer_group_enable_alarm_in_isr(TIMER_GROUP, TIMER_NUM);
}

// главная функция
void app_main()
{
    // создание буфера для хранения сэмплов
    buffer = (uint8_t *)calloc(DMA_BUF_LEN, sizeof(uint8_t));

    // инициализация I2S драйвера
    i2s_init();

    // инициализация таймера
    timer_init();
}
