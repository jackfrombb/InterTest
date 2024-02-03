#include <Arduino.h>

class DisplayBuffer
{
    uint8_t *buffer; // ваш буфер
    int width;       // ширина дисплея в пикселях
    int height;      // высота дисплея в пикселях
    bool bitOrder;   // порядок битов: true для младшего к старшему, false для старшего к младшему
    bool needFree = false;

    uint _bufferSize;

public:
    DisplayBuffer(uint bufferSize, int w, int h, bool order) : width(w), height(h), bitOrder(order)
    {
        buffer = (uint8_t *)calloc(bufferSize, sizeof(uint8_t));
        needFree = true;
        _bufferSize = bufferSize;
    }

    DisplayBuffer(uint8_t *buff, int w, int h, bool order) : buffer(buff), width(w), height(h), bitOrder(order)
    {
    }

    ~DisplayBuffer()
    {
        if (needFree)
        {
            free(buffer);
        }
    }

    void copyFrom(uint8_t *anoBuf)
    {
        memcpy(buffer, anoBuf, _bufferSize);
    }

    void copyTo(uint8_t *anoBuf)
    {
        memcpy(anoBuf, buffer, _bufferSize);
    }

    // Получение значения пикселя
    bool getPixel(int x, int y)
    {
        int pixel_number = y * width + x;
        int byte_index = pixel_number / 8;
        int bit_index = bitOrder ? pixel_number % 8 : 7 - pixel_number % 8;
        return buffer[byte_index] & (1 << bit_index);
    }

    // Установка значения пикселя
    void setPixel(int x, int y, bool value)
    {
        int pixel_number = y * width + x;
        int byte_index = pixel_number / 8;
        int bit_index = bitOrder ? pixel_number % 8 : 7 - pixel_number % 8;
        if (value)
        {
            buffer[byte_index] |= (1 << bit_index); // установка в 1
        }
        else
        {
            buffer[byte_index] &= ~(1 << bit_index); // установка в 0
        }
    }

    // Метод, который перебирает биты в массиве байт с заданной начальной точки до заданной конечной
    // Параметры:
    // arr - указатель на массив байт
    // n - размер массива в байтах
    // start - начальная точка в битах (от 0 до 8*n - 1)
    // end - конечная точка в битах (от start до 8*n - 1)
    // Функция возвращает количество единичных битов в заданном диапазоне
    int countBits(byte *arr, int n, int start, int end)
    {
        // Проверяем корректность входных данных
        if (arr == nullptr || n <= 0 || start < 0 || end < start || end >= 8 * n)
        {
            return -1; // Возвращаем -1 в случае ошибки
        }
        // Определяем индексы начального и конечного байтов в массиве
        int startByte = start / 8;
        int endByte = end / 8;
        // Определяем смещения начального и конечного битов в своих байтах
        int startBit = start % 8;
        int endBit = end % 8;
        // Инициализируем счетчик единичных битов
        int count = 0;
        // Перебираем байты в массиве от начального до конечного
        for (int i = startByte; i <= endByte; i++)
        {
            // Создаем маску для выбора нужных битов в текущем байте
            byte mask = 0xFF; // 11111111 в двоичном виде
            // Если это начальный байт, то обнуляем старшие биты до начального бита
            if (i == startByte)
            {
                mask >>= startBit; // Сдвигаем маску вправо на startBit позиций
            }
            // Если это конечный байт, то обнуляем младшие биты после конечного бита
            if (i == endByte)
            {
                mask <<= (7 - endBit); // Сдвигаем маску влево на 7 - endBit позиций
                mask >>= (7 - endBit); // Сдвигаем маску обратно вправо на 7 - endBit позиций
            }
            // Применяем маску к текущему байту и считаем количество единичных битов в результате
            byte result = arr[i] & mask; // Побитовое И между байтом и маской
            while (result > 0)
            {
                count += result & 1; // Прибавляем к счетчику младший бит результата
                result >>= 1;        // Сдвигаем результат вправо на одну позицию
            }
        }

        return count;
    }
};
