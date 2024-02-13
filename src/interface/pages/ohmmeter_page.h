#pragma once

class OhmmeterPage : public InterfacePageVirtual
{
private:
    OhmmeterPageView *_pageView;

public:
    OhmmeterPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new OhmmeterPageView(_display);
    }
    ~OhmmeterPage() override
    {
        delete _pageView;
        _pageView = nullptr;
    }

    PageView *getPageView() override
    {
        return _pageView;
    }
};