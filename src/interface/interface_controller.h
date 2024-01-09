#pragma once

#include "interface/pages/page_virtual.h"
#include "displays/display_virtual.h"
#include "interface/engines/interface_engine.h"
#include "interface/pages/page_list.h"

class InterfaceController
{
private:
    MainBoard* _mainBoard;
    DisplayVirtual *_display;

    InterfaceEngineVirtual *_interfaceEngine;

    InterfacePageVirtual *_currentPage = nullptr;

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
                controller->_drawInProcess = true;
            if (controller->_currentPage != nullptr)
            {
                controller->_interfaceEngine->drawPage(*controller->_currentPage);
            }
            else{
                Serial.println("Nothing to show");
            }
            controller->_drawInProcess = false;
            xTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

public:
    InterfaceController(MainBoard* mainBoard, InterfaceEngineVirtual *interfaceEngine)
    {
        _mainBoard = mainBoard;
        _display = mainBoard->getDisplay();
        _interfaceEngine = interfaceEngine;
    }

    ~InterfaceController()
    {
        clear();
        vTaskDelete(_interfaceTaskHandler);
    }

    float* showHelloPage()
    {
        clear();
        _currentPage = new HelloPage(_display);
        return ((HelloPage*)_currentPage)->getProgressPtr();
    }

    void showStartPage(){
        clear();
        _currentPage = new OscilPage(_mainBoard);
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