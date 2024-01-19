#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cmath>

// Класс для хранения данных с АЦП в буфере
class Buffer {
private:
    std::vector<double> data; // Массив для хранения данных
    std::mutex mtx; // Мьютекс для синхронизации доступа к данным
public:
    // Метод для добавления нового значения в буфер
    void add(double value) {
        std::lock_guard<std::mutex> lock(mtx); // Захватить мьютекс
        data.push_back(value); // Добавить значение в конец массива
    }

    // Метод для чтения данных из буфера
    std::vector<double> read() {
        std::lock_guard<std::mutex> lock(mtx); // Захватить мьютекс
        return data; // Вернуть копию данных
    }

    // Метод для очистки буфера
    void clear() {
        std::lock_guard<std::mutex> lock(mtx); // Захватить мьютекс
        data.clear(); // Очистить массив
    }
};

// Класс для синхронизации потоков
class Sync {
private:
    std::mutex mtx; // Мьютекс для синхронизации
    std::condition_variable cv; // Условная переменная для синхронизации
public:
    // Метод для оповещения о наличии данных в буфере
    void notify() {
        cv.notify_one(); // Оповестить один из ожидающих потоков
    }

    // Метод для ожидания данных в буфере
    void wait() {
        std::unique_lock<std::mutex> lock(mtx); // Захватить мьютекс
        cv.wait(lock); // Ожидать оповещения, освобождая мьютекс
    }
};

// Класс для обнаружения периодического сигнала в данных
class Detector {
private:
    double threshold; // Порог для определения пиков
public:
    // Конструктор с заданием порога
    Detector(double threshold) : threshold(threshold) {}

    // Метод для обнаружения сигнала в данных
    void detect(const std::vector<double>& data) {
        // Найти индексы пиков в данных, т.е. значения, которые больше порога и больше своих соседей
        std::vector<int> peaks;
        for (int i = 0; i < data.size(); i++) {
            if (data[i] > threshold && (i == 0  data[i] > data[i-1]) && (i == data.size() - 1  data[i] > data[i+1])) {
                peaks.push_back(i);
            }
        }

        // Если найдено хотя бы два пика, то считать, что есть периодический сигнал
        if (peaks.size() >= 2) {
            // Вычислить средний период сигнала как среднее расстояние между пиками
            double period = 0;
            for (int i = 1; i < peaks.size(); i++) {
                period += peaks[i] - peaks[i-1];
            }
            period /= (peaks.size() - 1);// Вычислить среднюю амплитуду сигнала как среднее значение пиков
            double amplitude = 0;
            for (int i = 0; i < peaks.size(); i++) {
                amplitude += data[peaks[i]];
            }
            amplitude /= peaks.size();

            // Вывести параметры сигнала на экран
            std::cout << "Периодический сигнал обнаружен!" << std::endl;
            std::cout << "Период: " << period << std::endl;
            std::cout << "Амплитуда: " << amplitude << std::endl;
        }
        else {
            // Иначе считать, что нет периодического сигнала
            std::cout << "Периодический сигнал не обнаружен." << std::endl;
        }
    }
};

// Функция для заполнения буфера данными с АЦП
void fill_buffer(Buffer& buffer, Sync& sync) {
    // Здесь должен быть код для получения данных с АЦП, например, через последовательный порт
    // Для простоты примера, будем генерировать случайные данные с некоторым шумом
    // Предположим, что есть периодический сигнал с периодом 10 и амплитудой 5
    srand(time(NULL)); // Инициализировать генератор случайных чисел
    for (int i = 0; i < 100; i++) { // Сгенерировать 100 значений
        double value = 5 * sin(2 * M_PI * i / 10) + (rand() % 10) / 10.0 - 0.5; // Случайное значение с шумом
        buffer.add(value); // Добавить значение в буфер
        sync.notify(); // Оповестить другой поток о наличии данных
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Сделать паузу в 100 мс
    }
}

// Функция для обработки данных из буфера и обнаружения сигнала
void process_data(Buffer& buffer, Sync& sync, Detector& detector) {
    while (true) { // Бесконечный цикл
        sync.wait(); // Ожидать данных в буфере
        std::vector<double> data = buffer.read(); // Прочитать данные из буфера
        detector.detect(data); // Обнаружить сигнал в данных
        buffer.clear(); // Очистить буфер
    }
}

int main() {
    Buffer buffer; // Создать объект буфера
    Sync sync; // Создать объект синхронизации
    Detector detector(3); // Создать объект детектора с порогом 3

    // Создать два потока: один для заполнения буфера, другой для обработки данных
    std::thread t1(fill_buffer, std::ref(buffer), std::ref(sync));
    std::thread t2(process_data, std::ref(buffer), std::ref(sync), std::ref(detector)); // Передать объекты по ссылке

    // Дождаться завершения потоков
    t1.join();
    t2.join();

    return 0;
}

// Функция для сравнения двух буферов и поиска смещения сигнала в них
// Принимает два одномерных массива данных с сигналом и их размер
// Возвращает смещение в индексах или 0, если буферы несинхронизируемые
int find_signal_offset(double buffer1[], double buffer2[], int size) {
  // Проверяем, что массивы не пустые
  if (size == 0) {
    return 0; // Возвращаем 0
  }

  // Проходим по первому буферу в цикле
  for (int i = 0; i < size; i++) {
    // Берем текущее значение из первого буфера
    double value = buffer1[i];

    // Ищем это значение во втором буфере
    for (int j = 0; j < size; j++) {
      // Если нашли совпадение, проверяем остальные значения
      if (buffer2[j] == value) {
        // Создаем флаг для проверки совпадения
        bool match = true;

        // Проходим по остальным значениям в цикле
        for (int k = 1; k < size; k++) {
          // Вычисляем индексы для сравнения
          int index1 = (i + k) % size; // Индекс для первого буфера с учетом цикличности
          int index2 = (j + k) % size; // Индекс для второго буфера с учетом цикличности

          // Сравниваем значения по индексам
          if (buffer1[index1] != buffer2[index2]) {
            // Если нашли различие, меняем флаг и выходим из цикла
            match = false;
            break;
          }
        }

        // Если флаг остался истинным, значит нашли совпадение буферов
        if (match) {
          // Возвращаем смещение в индексах
          return j - i;
        }
      }
    }
  }

  // Если не нашли совпадение буферов, возвращаем -1
  return 0;
}