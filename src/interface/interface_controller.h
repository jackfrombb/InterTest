#pragma once

#include "interface/pages/page_virtual.h"
#include "displays/display_virtual.h"
#include "interface/engines/interface_engine.h"
#include "interface/pages/page_list.h"

class InterfaceController
{
private:
    DisplayVirtual *_display;
    InterfacePageVirtual *_currentPage = nullptr;
    InterfaceEngineVirtual *_interfaceEngine;
    bool _drawInProcess = false;
    TaskHandle_t _interfaceTaskHandler;

    static void _drawInterfaceThread(void *pvParameters)
    {
        Serial.println("Interfae thread start");
        static TickType_t xLastWakeTime;
        static TickType_t xFrequency = 50;
        InterfaceController *controller = (InterfaceController *)pvParameters;

        xLastWakeTime = xTaskGetTickCount();
        while (1)
        {
            if (controller->_currentPage != nullptr)
            {
                controller->_drawInProcess = true;
                controller->_interfaceEngine->drawPage(*controller->_currentPage);
                controller->_drawInProcess = false;
            }
            xTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

public:
    InterfaceController(DisplayVirtual *display, InterfaceEngineVirtual *interfaceEngine)
    {
        _display = display;
        _interfaceEngine = interfaceEngine;
    }

    ~InterfaceController()
    {
        clear();
    }

    void showHelloPage(float *progress)
    {
        clear();
        _currentPage = new HelloPage(_display, progress);
    }

    void clear()
    {
        if (_currentPage != nullptr)
        {
            delete _currentPage;
        }
    }

    void init()
    {
        // Прикрепить процесс к ядру
        xTaskCreatePinnedToCore(
            _drawInterfaceThread,   // Function to implement the task
            "drawInterfaceThread",  // Name of the task
            2048,                   // Stack size in bytes
            this,                   // Task input parameter
            10,                     // Priority of the task
            &_interfaceTaskHandler, // Task handle.
            tskNO_AFFINITY          // Core where the task should run
        );
    }
};