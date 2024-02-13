#pragma once

class SettingsPageView : public PageView
{
private:
    ElBattery batteryIndicator;
public:
    SettingsPageView(DisplayVirtual *display) : PageView(display)
    {
        
    }
};