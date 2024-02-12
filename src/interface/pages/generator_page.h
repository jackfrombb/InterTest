#pragma once

class GeneratorPage : public InterfacePageVirtual
{
private:
    GeneratorPageView *_pageView;

public:
    GeneratorPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        _pageView = new GeneratorPageView(_display);
    }
    ~GeneratorPage() override
    {
        delete _pageView;
        _pageView = nullptr;
    }

    virtual PageView *getPageView()
    {
        return _pageView;
    }
};