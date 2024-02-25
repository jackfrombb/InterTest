/*

    Пока не используется

*/
#pragma once

#include "interface/ellements/ellements_list.h"
#include "share_setting.h"
#include "displays/display_virtual.h"
#include "interface/localisation/en.h"

/// @brief Фрагмент настройки для переключения состояний (Вкл-Выкл)
class StateSettingsFragment : public ElGroup
{
private:
    setting_args_bool *_args;
    ShareSetting *_setting;

public:
    // Название настройки
    ElText title;
    ElTextButton state;

    StateSettingsFragment(DisplayVirtual *display, ShareSetting *setting, uint8_t *selectedPtr)
    {
        _setting = setting;
        _args = (setting_args_bool *)setting->getArgs();

        title.setText(_setting->getName())
            ->setWidthMatchParent()
            ->setY(0);

        state.setSelectedButtonPtr(selectedPtr)
            ->setCalculatedText([this]
                                { return _args->currentVal ? LOC_ON : LOC_OFF; })
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(display->getMaxTextHeight(title.getTextSize()) + 10);

        addElement(&title)->addElement(&state);
    }

    bool getState()
    {
        return _args->currentVal;
    }
};