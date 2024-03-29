/**
 * @file share_setting.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Класс и вспомогательные структуры для передачи настроек разного типа и назначения
 * на страницу настроек и отслеживания их изменений
 * @version 0.1
 * @date 2024-02-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <Arduino.h>
#include <vector>
using namespace std;

/// @brief Тип настроек. С помощью этой структуры можно выполнить приведение типов для аргументов
enum share_setting_type
{
    // через передачу целого числа в заданных пределах (прим: от 0 до 255)
    // ограничение задаётся в структуре setting_args_int_range
    SETTING_TYPE_INT_RANGE,
    // через передачу одного из доступных целочисленных значений (прим: [0, 2, 10, 50, 100])
    // значения задаются в структуре setting_args_int_steep
    SETTING_TYPE_INT_STEEP,
    // через передачу состояния true или false (прим: вкл/выкл)
    // для обозначения типа используется структура setting_args_bool
    SETTING_TYPE_BOOL
};

/// @brief Общая, виртуальная основа для вспомогательных, ограничительных структур
/// необходимо привести к нужному типу используя share_setting_type.

struct settings_args_virtual
{
public:
    int16_t id = 0; // Id для определения настройки в событии (используется, когда несколько настроек обрабатываются одним методом)
    // тип настройки
    share_setting_type settings_type;
    // активна ли настройка (возможно какие то другие настройки её отключают её действие)
    bool isEnabled = true;
    // тег для доступа в пзу
    const char *inRomTag;

    // Получить ссылку на значение
    virtual void *getValuePtr() = 0;

    // Сохранить значение в пзу
    virtual void saveToRom()
    {
        switch (settings_type)
        {
        case share_setting_type::SETTING_TYPE_BOOL:
        {
            AppData::get()->saveBool(inRomTag, *((bool *)getValuePtr()));
            break;
        }

        case share_setting_type::SETTING_TYPE_INT_STEEP:
        {
            uint val = *((uint *)getValuePtr());
            //Serial.println("Val: " + String(val));
            AppData::get()->saveUInt(inRomTag, val);
            break;
        }
        case share_setting_type::SETTING_TYPE_INT_RANGE:
        {
            AppData::get()->saveInt(inRomTag, *((int *)getValuePtr()));
            break;
        }
        }
    }

protected:
    /// @brief Конструктор. Скрыт для не наследующих классов, что бы не могли создать объект
    /// @param inRomTag Тег для хранения настройки в пзу
    settings_args_virtual(int16_t idNum, const char *tag)
    {
        id = idNum;
        inRomTag = tag;
    }
};

/// @brief Настройки для bool типов
struct setting_args_bool : public settings_args_virtual
{
    bool currentVal;

    setting_args_bool(int16_t id, const char *inRomTag, bool defaultVal) : settings_args_virtual(id, inRomTag)
    {
        settings_type = share_setting_type::SETTING_TYPE_BOOL;
        currentVal = AppData::getBool(inRomTag, defaultVal);
    }

    void *getValuePtr() override
    {
        return &currentVal;
    }
};

/// @brief Ограничение для SETTING_TYPE_INT_RANGE
struct setting_args_int_range : public settings_args_virtual
{
    int fromVal;
    int toVal;
    int currentVal;

    /// @brief Конструтор
    /// @param from начало ограничения включительно
    /// @param to конец ограничения включительно
    /// @param currVal текущее значение
    setting_args_int_range(int16_t id, const char *inRomTag, int from, int to, int defaultVal) : settings_args_virtual(id, inRomTag)
    {
        settings_type = share_setting_type::SETTING_TYPE_INT_RANGE;
        fromVal = from;
        toVal = to;
        currentVal = AppData::getInt(inRomTag, defaultVal);
    }

    void *getValuePtr() override
    {
        return &currentVal;
    }
};

/// @brief Задача значений для SETTING_TYPE_INT_STEEP
struct setting_args_int_steep : public settings_args_virtual
{
    vector<int> *values;
    uint currentVal = 0; // Индекс текущего значения

    setting_args_int_steep(int16_t id, const char *inRomTag, vector<int> *vals, int defaultVal) : settings_args_virtual(id, inRomTag)
    {
        values = vals;
        settings_type = share_setting_type::SETTING_TYPE_INT_STEEP;
        currentVal = range(AppData::getUint(inRomTag, defaultVal), 0, values->size() - 1);
    }

    void increaseCurrentVal()
    {
        currentVal = currentVal + 1;
        if (currentVal >= values->size())
        {
            currentVal = 0;
        }
        // logi::p("Setting steep", "Intrease. Size: " + String(values->size()) + " val: " + String(currentVal));
    }

    void decreaseCurrentVal()
    {
        int val = (int)currentVal - 1;
        if (val < 0)
        {
            val = values->size() - 1;
        }
        currentVal = val;
    }

    /// @brief Возвращает значение по адресу currentVal из массива значений
    /// @return
    int getSteepValue()
    {
        if (isHaveValue())
        {
            return (*values)[min<uint>(currentVal, values->size() - 1)];
        }
        logi::p("Seting steep", "dont have steep value");
        return 0;
    }

    void *getValuePtr() override
    {
        return &currentVal;
    }

    /// @brief Проверка на наличие валидного значения
    /// @return true если кол-во шагов больше 0, есть ссылка на массив и выбранный индекс меньше максимального
    bool isHaveValue()
    {
        return values != nullptr && values->size() > 0;
    }
};

/// @brief Класс для предачи настройки чего либо от любого модуля на страничку настроек и возможности отслеживания их изменений
class ShareSetting
{
private:
    String _name = "err_03";                               // Название для заголовка
    settings_args_virtual *_args;                          // Аргументы с деталями настройки
    function<bool(settings_args_virtual *args)> _onChange; // Событие оповещающее владельца настройки об её изменении, возвращает true если настройки применены успешно

protected:
public:
    /// @brief Конструктор
    /// @param name  Название для заголовка на страничек настроек
    /// @param args  Аргументы с деталями настройки
    /// @param onChange Событие оповещающее владельца настройки об её изменении
    ShareSetting(String name, settings_args_virtual *args, function<bool(settings_args_virtual *args)> onChange)
    {
        _name = name;
        _args = args;
        _onChange = onChange;
    }

    const String getName()
    {
        return _name;
    }

    const settings_args_virtual *getArgs()
    {
        return _args;
    }

    void onChange()
    {
        if (_onChange(_args))
        {
            _args->saveToRom();
        }
    }
};

/// @brief Интерфейс/класс для классов имеющих настройки
class iHaveShareSettings
{
    String _ownerName;
    vector<ShareSetting *> settings;

public:
    iHaveShareSettings(String ownerName)
    {
        _ownerName = ownerName;
    }
    ~iHaveShareSettings()
    {
        settings.clear();
    }

    virtual vector<ShareSetting *> getSettings() { return settings; }
    virtual String getShareSettingsOwnerName() { return _ownerName; };
    virtual iHaveShareSettings *addSetting(ShareSetting *setting)
    {
        settings.push_back(setting);
        return this;
    }
};
