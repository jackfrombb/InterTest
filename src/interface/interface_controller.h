/**
 * @file interface_controller.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Управление отображаемыми страничками. Отрисовка интерфейса в отдельном потоке.
 * @version 0.1
 * @date 2024-02-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

// #include "interface/pages/page_virtual.h"
// #include "displays/display_virtual.h"
// #include "interface/engines/interface_engine.h"
// #include "interface/pages/page_list.h"
// #include "logi.h"

class InterfaceController
{
private:
    MainBoard *_mainBoard;
    DisplayVirtual *_display;

    InterfaceEngineVirtual *_interfaceEngine;
    bool _startClear = false;

    InterfacePageVirtual *_currentPage = nullptr;

    bool _drawInProcess = false;

    TaskHandle_t _interfaceTaskHandler;

    static void _drawInterfaceThread(void *pvParameters)
    {
        logi::p("Interface controller", "Interface thread start");

        static TickType_t xLastWakeTime;
        static TickType_t xFrequency = 30;
        auto *controller = (InterfaceController *)pvParameters;

        xLastWakeTime = xTaskGetTickCount();
        while (1)
        {
            controller->_drawInProcess = true;
            if (!controller->_startClear &&
                controller->_currentPage != nullptr &&
                controller->_currentPage->getPageView() != nullptr)
            {
                controller->_currentPage->onDraw();                                              // Оповещаем контроллер страницы для подготовки данных
                controller->_interfaceEngine->drawPage(controller->_currentPage->getPageView()); // Отображаем данные
            }
            controller->_drawInProcess = false;
            xTaskDelayUntil(&xLastWakeTime, xFrequency);
        }
    }

    static bool _controlEvent(control_event_type eventType, void *args)
    {
        InterfaceController *iController = (InterfaceController *)args;

        if ((iController->_currentPage == nullptr || !iController->_currentPage->onControlEvent(eventType)) &&
            eventType == control_event_type::PRESS_BACK)
        {
            logi::p("iController", "Pressed back to main menu");
            iController->showMainMenu();
        }

        return true;
    }

public:
    InterfaceController(MainBoard *mainBoard)
    {
        _mainBoard = mainBoard;
        _display = mainBoard->getDisplay();
        _interfaceEngine = _display->getInterfaceEngine();

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
        showMainMenu();
    }

    void showMainMenu()
    {
        clear();
        logi::p("iController", "Before start page");
        setCurrentPage(new StartPage(_mainBoard->getDisplay(),
                                     [this](pages_list p)
                                     { onPageSelected(p); }));
        // setCurrentPage(new OscilPage(_mainBoard));
    }

    /// @brief Событие перехода на другую страницу
    /// @param page Выбранная страница
    void onPageSelected(pages_list page)
    {
        logi::p("Inerface_Controller", "Select " + String(page));

        switch (page)
        {
        case pages_list::PAGE_LIST_OSCIL:
        {
            clear();
            setCurrentPage(new OscilPage(_mainBoard));
        }
        break;

        case pages_list::PAGE_LIST_VOLT:
            clear();
            setCurrentPage(new VoltmeterPage(_mainBoard));
            break;

        case pages_list::PAGE_GENERATOR:
            clear();
            setCurrentPage(new GeneratorPage(_mainBoard));
            break;

        case pages_list::PAGE_SETTINGS:
            clear();
            setCurrentPage(new SettingsPage(_mainBoard));
            break;
        }
    }

    void setCurrentPage(InterfacePageVirtual *page)
    {
        if (page == nullptr)
        {
            logi::err("iController", "SELECT NULL PAGE");
        }
        _currentPage = page;
        delayMicroseconds(1000); // Ждем пока инициализируется следующая страница, иначе вызывает искючение
        _startClear = false;
        logi::p("iController", "End clear. Page == nullptr : " + String(_currentPage == nullptr));
    }

    void clear()
    {
        _startClear = true;
        if (_currentPage != nullptr)
            _currentPage->onClose();
        delete _currentPage;
        _currentPage = nullptr;
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