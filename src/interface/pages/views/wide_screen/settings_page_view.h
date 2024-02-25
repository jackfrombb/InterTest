#pragma once

class SettingsPageView : public PageView
{
private:
    uint8_t _selectedButton;
    uint8_t buttonsCount;

    iHaveShareSettings **_settingsOwners;
    uint8_t _ownersCount;

    void _createSettingsSection(iHaveShareSettings **owners, uint8_t count)
    {
        ElScroll *root = new ElScroll();
        root->setWidth(_display->getWidth());

        for (int i = 0; i < count; i++)
        {
            iHaveShareSettings *owner = owners[i];

            int y = 0;
            if (i > 0)
            {
                ElementVirtual *prevEl = root->getElements()[root->getElementsCount() - 1];
                y = prevEl->getY() + prevEl->getHeight();
            }

            ElText *sectionTitle = new ElText(owner->getShareSettingsOwnerName());

            sectionTitle
                ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
                ->setWidthMatchParent()
                ->setY(y)
                ->setHeight(_display->getMaxTextHeight(sectionTitle->getTextSize()) + 10);

            root->addElement(sectionTitle);

            for (auto s : owner->getSettings())
            {
                int y = 0;
                if (root->getElements().size() > 0)
                {
                    ElementVirtual *prevEl = root->getElements()[root->getElements().size() - 1];
                    y = prevEl->getY() + prevEl->getHeight();
                }

                ElText *settingTitle = new ElText(s->getName());
                settingTitle
                    ->setY(y)
                    ->setHeight(_display->getMaxTextHeight(settingTitle->getTextSize()));

                _addSettingToRoot(root, settingTitle, s);
            }
        }

        addElement(root);
    }

    void _addSettingToRoot(ElGroup *root, ElText *title, ShareSetting *setting)
    {
        ElTextButton *button = new ElTextButton();
        button
            ->setButtonId(buttonsCount++)
            ->setSelectedButtonPtr(&_selectedButton)
            ->setY(title->getY() + title->getHeight())
            ->setHeight(_display->getMaxTextHeight(button->getTextSize()));

        switch (setting->getArgs()->settings_type)
        {
        case share_setting_type::SETTING_TYPE_BOOL:
        {
            button
                ->setCalculatedText([this](void *arg)
                                    {
                ShareSetting* setting = (ShareSetting*) arg;
                auto args = (setting_args_bool *) setting->getArgs();
                return args->currentVal ? LOC_ON : LOC_OFF; },
                                    setting);
            break;
        }

        case share_setting_type::SETTING_TYPE_INT_RANGE:
        {
            button->setCalculatedText([this](void *arg)
                                      {
                ShareSetting* setting = (ShareSetting*) arg;
                auto args = (setting_args_int_range *) setting->getArgs();
                return String(args->currentVal); },
                                      setting);
            break;
        }

        case share_setting_type::SETTING_TYPE_INT_STEEP:
        {
            button
                ->setCalculatedText([this](void *arg)
                                    {
                ShareSetting* setting = (ShareSetting*) arg;
                auto args = (setting_args_int_range *) setting->getArgs();
                return String(args->currentVal); },
                                    setting);
            break;
        }
        }

        Serial.println("Button Added");
        root->addElement(button);
    }

    void _clearElements(ElGroup *group)
    {
        for (auto el : group->getElements())
        {
            if (el->isGroup())
            {
                _clearElements((ElGroup *)el);
            }

            delete el;
            el = nullptr;
        }
    }

public:
    SettingsPageView(DisplayVirtual *display, iHaveShareSettings **settingsOwners, uint8_t ownersCount) : PageView(display)
    {
        _settingsOwners = settingsOwners;
        _ownersCount = ownersCount;
        _createSettingsSection(_settingsOwners, ownersCount);
    }

    ~SettingsPageView()
    {
        _clearElements(this);
        getElements().clear();
    }
};