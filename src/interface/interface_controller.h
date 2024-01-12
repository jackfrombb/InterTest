#pragma once

#include "interface/pages/page_virtual.h"
#include "displays/display_virtual.h"
#include "interface/engines/interface_engine.h"
#include "interface/pages/page_list.h"
#include "logi.h"

class InterfaceController
{
private:
    MainBoard *_mainBoard;
    DisplayVirtual *_display;

    InterfaceEngineVirtual *_interfaceEngine;
    bool _startClear = false;

    InterfacePageVirtual *_currentPage = nullptr;
    InterfacePageVirtual *_preparePage = nullptr; // Для ожидания инициализации основной страницы

    bool _drawInProcess = false;

    TaskHandle_t _interfaceTaskHandler;

    static void _drawInterfaceThread(void *pvParameters)
    {
        logi::p("Interface controller", "Interface thread start");

        static TickType_t xLastWakeTime;
        static TickType_t xFrequency = 50;
        InterfaceController *controller = (InterfaceController *)pvParameters;

        xLastWakeTime = xTaskGetTickCount();
        while (1)
        {
            controller->_drawInProcess = true;
            if (!controller->_startClear && controller->_currentPage != nullptr)
            {
                controller->_currentPage->onDraw(millis());
                controller->_interfaceEngine->drawPage(controller->_currentPage->getPageView());
            }
            controller->_drawInProcess = false;
            xTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

    static void _controlEvent(control_event_type eventType, void *args)
    {
        InterfaceController* iController = (InterfaceController*) args;
        if(iController->_currentPage != nullptr){
            iController->_currentPage->onControlEvent(eventType);
        }
    }

public:
    InterfaceController(MainBoard *mainBoard, InterfaceEngineVirtual *interfaceEngine)
    {
        _mainBoard = mainBoard;
        _display = mainBoard->getDisplay();
        _interfaceEngine = interfaceEngine;
        
        // Подключаем упарвление
        _mainBoard->getControl()->attachControlHandler(_controlEvent, this);
    }

    ~InterfaceController()
    {
        clear();
        vTaskDelete(_interfaceTaskHandler);
    }

    float *showHelloPage()
    {
        clear();
        setCurrentPage(new HelloPage(_display));
        return ((HelloPage *)_currentPage)->getProgressPtr();
    }

    void start()
    {
        clear();
        setCurrentPage(new OscilPage(_mainBoard));
    }

    void setCurrentPage(InterfacePageVirtual* page){
        _currentPage = page;
        _startClear = false;
    }

    void clear()
    {
        _startClear = true;
        if (_currentPage != nullptr)
        {
            delete _currentPage;
            _currentPage = nullptr;
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