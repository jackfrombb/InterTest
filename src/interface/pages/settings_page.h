/**
 * @file settings_page.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Страница общих настроек для устройства
 * @version 0.1
 * @date 2024-02-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

class SettingsPage : public InterfacePageVirtual
{
private:
    SettingsPageView* _pageView;

public:
    SettingsPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new SettingsPageView(_display);
    }

    ~SettingsPage()
    {
        delete _pageView;
        _pageView = nullptr;
    }

    virtual PageView *getPageView()
    {
        return _pageView;
    }
};