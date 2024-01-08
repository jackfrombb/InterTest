#pragma once

#include "interface/pages/page_virtual.h"
#include "displays/display_virtual.h"

class InterfaceController
{
private:
    InterfacePageVirtual *_currentPage;
    InterfaceEngineVirtual *_interfaceEngine;

public:
    InterfaceController(DisplayVirtual *display)
    {
        
    }
    
    ~InterfaceController()
    {
        if (_currentPage != nullptr)
            delete _currentPage;
    }

    void init()
    {

    }
};