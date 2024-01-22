#pragma once
#include "page_virtual.h"
#include "board_virtual.h"
#include "interface/pages/views/wide_screen/voltmeter_page_view.h"

class VoltmeterPage : public InterfacePageVirtual
{
private:
    VoltemeterPageView* pageView;

public:
    VoltmeterPage(MainBoard *mainBoard) : InterfacePageVirtual(mainBoard->getDisplay())
    {
        pageView = new VoltemeterPageView(mainBoard->getDisplay());
    }

    ~VoltmeterPage(){
        delete pageView;
    }

    
    virtual PageView *getPageView() {
        return pageView;
    }
};