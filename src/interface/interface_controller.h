#pragma once

#include "interface/pages/page_virtual.h"
#include "displays/display_virtual.h"
#include "interface/engines/interface_engine.h"

class InterfaceController
{
private:
    InterfacePageVirtual *_currentPage = nullptr;
    InterfaceEngineVirtual *_interfaceEngine;
    bool _drawInProcess = false;

public:
    InterfaceController(InterfaceEngineVirtual* interfaceEngine)
    {
        _interfaceEngine = interfaceEngine;
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