#pragma once

class SettingsPageView : public PageView
{
private:
    // номер кнопки на который наведен фокус
    uint8_t _selectedButton = 0;
    // Флаг нахождения в режиме редактирования
    // flase - normal, true - edit
    bool _inEditMode = false;

    // Список точек перемотки для кнопок (id кнопки = положение в списке, значение = положение в ElScroll root)
    vector<uint8_t> _scrollPositions;
    // Список соотношений позиции кнопки в root и её id.(положение в списке = id кнопки, значение = положение кнопки в root)
    // _buttonPositions.size() = кол-во кнопок
    vector<uint8_t> _buttonPositions;

    // Массив указателей классов-владельцев настроек
    iHaveShareSettings **_settingsOwners;
    // Размер массива _settingsOwners
    uint8_t _ownersCount;

    // Корневой элемент
    ElScroll *root;

    /// @brief Заполнить страничку секциями и настройками в них
    /// @param owners владельцы настроек (название секции)
    /// @param count кол-во владельцев
    void _createSettingsSection(iHaveShareSettings **owners, uint8_t count)
    {
        int y = 0; // Для высчитывания положения следующего элемента по y

        root = new ElScroll(false);
        root
            ->setWidth(_display->getWidth())
            ->setY(y);

        for (int i = 0; i < count; i++)
        {
            iHaveShareSettings *owner = owners[i];

            // Добавляем заголовок секции, с названием настраиваемого класса
            ElText *sectionTitle = new ElText(owner->getShareSettingsOwnerName());

            sectionTitle
                ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
                ->setVerticalAlignment(el_vertical_align::EL_TEXT_ALIGN_SELF_CENTER)
                ->setWidthMatchParent()
                ->setY(y)
                ->setHeight(_display->getMaxTextHeight(sectionTitle->getTextSize()) + 6)
                ->setVisualStyleFlags(STYLE_MAIN_TITLE);

            // Добавляем позицию прокрутки и элемент в списки
            _scrollPositions.push_back(root->getElementsCount());
            root->addElement(sectionTitle);

            // Увеличиваем на высоту
            y += sectionTitle->getHeight();
            if (i > 0)
            {
                y += 6; // если не первый элемент, то добавляем отступ
            }

            // Флаг, что предыдущим добавлялся главный заголовок
            bool prevSectionTitle = true;
            // Перебираем настройки
            for (auto s : owner->getSettings())
            {
                // отступ от заголовка элемента 4px
                if (prevSectionTitle)
                {
                    prevSectionTitle = false;
                    y += 4; // Отступ от заголовка секции
                    // тут добавлять позицию прокрутки не надо, поскольку она уже добавлена на заголовок секции
                }
                else
                {
                    _scrollPositions.push_back(root->getElementsCount()); // Добавляем позицию прокрутки

                    y += 6; // Отступ от предыдущего пункта
                }

                // Добавляем название настройки
                ElText *settingTitle = new ElText(s->getName() + ":");

                settingTitle
                    ->setY(y)
                    ->setWidthMatchParent()
                    //->setVisualStyleFlags(STYLE_ROUNDED_BORDER_AROUND) // Для визуального контроля при отладке
                    ->setHeight(_display->getMaxTextHeight(settingTitle->getTextSize()));

                root->addElement(settingTitle);

                // Увеличиваем на высоту + отступ для кнопки
                y += settingTitle->getHeight() + 6;

                _addButtonToSetting(root, &y, s);
            }
        }

        addElement(root);
    }

    /// @brief Добавить кнопку для настройки
    /// @param root корневой элемент
    /// @param y положение добавляемого элемента по y
    /// @param setting настройка
    void _addButtonToSetting(ElGroup *root, int *y, ShareSetting *setting)
    {
        ElTextButton *button = new ElTextButton();

        button
            ->setButtonId(_buttonPositions.size()) // Как id ставим позицию в списке позиций кнопок
            ->setSelectedButtonPtr(&_selectedButton)
            ->setShareSetting(setting)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_PARENT)
            ->setY(*y) // отступ на 8 писелей, что бы рамка не наезжала
            ->setHeight(_display->getMaxTextHeight(button->getTextSize()))
            ->setWidthMatchParent();

        // Добавляем вычисляемый текст в зависимости от типа нстройки
        switch (setting->getArgs()->settings_type)
        {
        case share_setting_type::SETTING_TYPE_BOOL:
        {
            button
                // Высчитыываемый текст
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
            button
                // Устанавливаем вычисляемый текст
                ->setCalculatedText([this](void *arg)
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
                auto args = (setting_args_int_steep *) setting->getArgs();
                return String(args->getSteepValue()); },
                                    setting);
            break;
        }
        }

        // Добавляем сведения о позиции в список позиций кнопок и кнопку в корневую группу
        _buttonPositions.push_back(root->getElementsCount());
        root->addElement(button);

        // увеличиваем на высоту
        *y += button->getHeight();
    }

    /// @brief Очистить список от new
    /// @param group корневой элемент НЕ включительно
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

    /// @brief Получить крайний в root элемент (Указатель)
    ElementVirtual *_getLastRootElement()
    {
        return root->getElements()[root->getElements().size() - 1]; // Аккуратнее с пустым списком
    }

    /// @brief отправить событие управления на кнопку
    /// @param isEventProcessed флаг обработано ли событие
    /// @param event событие управления
    void _controlEventInEditMode(bool *isEventProcessed, control_event_type event)
    {
        // Выбираем кнопку из списка и отправляем ей событие
        ElTextButton *button = (ElTextButton *)root->getElement(_buttonPositions[_selectedButton]);
        *isEventProcessed = button->onControl(event);
        _inEditMode = button->isInEditMode();
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

    /// @brief Событие от управления
    /// @param eventType тип события
    /// @return true если событие обработано
    bool onControlEvent(control_event_type eventType) override
    {
        // Флаг обработки события управления
        bool eventProcessed = false;

        // Если в режиме редактирования то отправляем обработку в другой метод иначе переход по кнопкам
        if (_inEditMode)
        {
            _controlEventInEditMode(&eventProcessed, eventType);
        }
        else
        {
            switch (eventType)
            {
            case control_event_type::PRESS_LEFT:                                              // Переход по кнопкам назад
                _selectedButton = range(_selectedButton - 1, 0, _buttonPositions.size() - 1); // Переводим фокус на кнопку
                root->smoothScrollToPosition(_scrollPositions[_selectedButton]);              // Переводим скролл на заголовок кнопки
                eventProcessed = true;                                                        // отмечаем, что обработали событие контрола
                break;

            case control_event_type::PRESS_RIGHT: // Переход по кнопкам вперед
                _selectedButton = range(_selectedButton + 1, 0, _buttonPositions.size() - 1);
                root->smoothScrollToPosition(_scrollPositions[_selectedButton]);
                eventProcessed = true;
                break;
            case control_event_type::LONG_PRESS_LEFT:
            case control_event_type::LONG_PRESS_RIGHT:
            case control_event_type::PRESS_OK:
                _inEditMode = true;
                onControlEvent(eventType); // отправляем нажатие кнопке
                break;
            }
        }

        return eventProcessed;
    }
};