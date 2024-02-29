#pragma once
#include <Arduino.h>
#include "helpers.h"

/// @brief Абстракция над контроллером ШИМ генерации
class PwmControllerVirtual : public iHaveShareSettings
{
private:
protected:
    uint8_t _dacPin = 0;
    // Настройки состояния
    setting_args_bool *_stateArg = nullptr; // = setting_args_bool(0, "ge_st", true);
    ShareSetting *_stateSetting = nullptr;  // = ShareSetting(LOC_STATE, _stateArg, _onSettingChangeEvent);

    // Настройки частоты
    setting_args_int_range *_freqArg = nullptr; // = setting_args_int_range(1, "ge_frq", 0, 20000000, 100000);
    ShareSetting *_freqSetting = nullptr;       // ShareSetting(LOC_FREQ, _freqArg, _onSettingChangeEvent);

    // Настройки скважности
    vector<int> dutySteeps;                     // Шаги скважности
    setting_args_int_steep *_dutyArg = nullptr; // = setting_args_int_steep(2, "ge_dty", {}, 0);
    ShareSetting *_dutySetting = nullptr;       // ShareSetting(LOC_DUTY, _dutyArg, _onSettingChangeEvent);

    // Событие изменения натсроек
    function<bool(settings_args_virtual *)> _onSettingChangeEvent = [this](settings_args_virtual *args)
    {
        bool isSuccesfull = true;
        switch (args->id)
        {
        case 0:
            setEnable(_stateArg->currentVal);
            break;

        case 2:
        case 1:
            _dutyArg->currentVal = _calculateDutySteeps();
            isSuccesfull = applySetting();
            break;
        }

        return isSuccesfull;
    };

    PwmControllerVirtual(uint8_t dacPin) : iHaveShareSettings(LOC_GENERATOR)
    {
        _dacPin = dacPin;
    }

    virtual uint8_t _calculateDutySteeps() = 0;

public:
    virtual ~PwmControllerVirtual()
    {

        delete _stateArg;
        _stateArg = nullptr;
        delete _freqArg;
        _freqArg = nullptr;
        delete _dutyArg;
        _dutyArg = nullptr;

        delete _stateSetting;
        _stateSetting = nullptr;
        delete _freqSetting;
        _freqSetting = nullptr;
        delete _dutySetting;
        _dutySetting = nullptr;
    }

    virtual void settingsInit()
    {
        // Настройки состояния вкл-выкл
        _stateArg = new setting_args_bool(0, "ge_st", true);
        // Настройки частоты
        _freqArg = new setting_args_int_range(1, "ge_frq", 0, 20000000, 100000);
        // Вычисляем среднее положение для настроек скважности
        uint currentSteep = (uint)_calculateDutySteeps();
        _dutyArg = new setting_args_int_steep(2, "ge_dty", &dutySteeps, currentSteep);

        // Serial.println("Steeps count : " + String(dutySteeps.size()) + " CurSteep: " + String(currentSteep));

        _stateSetting = new ShareSetting(LOC_STATE, _stateArg, _onSettingChangeEvent);
        _freqSetting = new ShareSetting(LOC_FREQ, _freqArg, _onSettingChangeEvent);
        _dutySetting = new ShareSetting(LOC_DUTY, _dutyArg, _onSettingChangeEvent);

        // Применяем настроки
        applySetting();

        // Добавляем настройки в общий список
        addSetting(_stateSetting)->addSetting(_freqSetting)->addSetting(_dutySetting);
    }

    virtual void setEnable(bool enabled)
    {
        _stateArg->currentVal = enabled;
        applySetting();
    }

    virtual ShareSetting *getStateSetting()
    {
        return _stateSetting;
    }
    virtual ShareSetting *getFreqSetting()
    {
        return _freqSetting;
    }
    virtual ShareSetting *getDutySetting()
    {
        return _dutySetting;
    }

    virtual bool isEnable()
    {
        return _stateArg->currentVal;
    }

    virtual void startMeander() = 0;
    virtual void stopMeander() = 0;
    virtual bool applySetting() = 0;
};
