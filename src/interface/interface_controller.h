/**
 * @file interface_controller.h
 * @author JackFromBB (jack@boringbar.ru)
 * @brief Управление страничками. Отрисовка интерфейса в отдельном потоке.
 * По сути, является главным потоком из трёх (опрос контроля(кнопок, энкодера и пр.), отрисовка, измерение),
 * поскольку все данные запрашиваются и предназначены именно для потока отрисовки
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

/// @brief  Управление страничками. Отрисовка интерфейса в отдельном потоке.
class InterfaceController
{
private:
    MainBoard *_mainBoard;
    DisplayVirtual *_display;

    TickType_t xLastWakeTime;
    TickType_t xFrequency = 30;

    InterfaceEngineVirtual *_interfaceEngine;
    bool _startClear = false;

    InterfacePageVirtual *_currentPage = nullptr;

    bool _drawInProcess = false;

    TaskHandle_t _interfaceTaskHandler = NULL;

    static void _drawInterfaceThread(void *pvParameters)
    {
        logi::p("Interface controller", "Interface thread start");
        auto *controller = (InterfaceController *)pvParameters;
        controller->xLastWakeTime = xTaskGetTickCount();
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
            xTaskDelayUntil(&controller->xLastWakeTime, controller->xFrequency);
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
        removeCurrentPage();
    }

    /// @brief Показать страничку загрузки (самая начальная)
    /// Нужна для приветствия и отслеживания процесса загрузки
    /// @return процесс загрузки (min:0.0; max:1.0)
    float *showHelloPage()
    {
        setCurrentPage(new HelloPage(_display));
        return ((HelloPage *)_currentPage)->getProgressPtr();
    }

    /// @brief Точка входа в основной интерфейс
    void start()
    {
        showMainMenu();
    }

    /// @brief Отобразить главное меню
    void showMainMenu()
    {
        setCurrentPage(new StartPage(_mainBoard->getDisplay(),
                                     [this](pages_list p)
                                     { onPageSelected(p); }));
    }

    /// @brief Событие перехода на другую страницу
    /// @param page Выбранная страница
    void onPageSelected(pages_list page)
    {
        //logi::p("Inerface_Controller", "Select " + String(page));
        switch (page)
        {
        case pages_list::PAGE_LIST_OSCIL:
        {
            setCurrentPage(new OscilPage(_mainBoard));
        }
        break;

        case pages_list::PAGE_LIST_VOLT:
            setCurrentPage(new VoltmeterPage(_mainBoard));
            break;

        case pages_list::PAGE_GENERATOR:
            setCurrentPage(new GeneratorPage(_mainBoard));
            break;

        case pages_list::PAGE_SETTINGS:
            setCurrentPage(new SettingsPage(_mainBoard));
            break;
        }
    }

    /// @brief Установить текущую страничку
    /// @param page Класс страницы
    void setCurrentPage(InterfacePageVirtual *page)
    {
        // Удаляем текущую страничку
        removeCurrentPage();

        // Устанавливаем новую страничку
        _currentPage = page; 

        // [Исправил] Ждем пока инициализируется следующая страница, иначе иногда бывает искючение
        // delayMicroseconds(1000);

        // Выключаем флаг, запрещающий отрисовку
        _startClear = false;
    }

    /// @brief Удалить текущую страничку с особождением ресурсов
    void removeCurrentPage()
    {
        // Включаем флаг старта удаления странички, что бы запретить отрисовку
        _startClear = true;

        // Ждем завершения отрисовки в потоке, если она запущена
        while (_drawInProcess)
        {
            vTaskDelay(5);
        }

        // Оповещаем страничку событием
        if (_currentPage != nullptr)
            _currentPage->onClose();

        // Удаляем страничку
        delete _currentPage;
        _currentPage = nullptr;
    }

    void stopDrawThread()
    {
        if (_interfaceTaskHandler != nullptr && _interfaceTaskHandler != NULL)
        {
            auto state = eTaskGetState(_interfaceTaskHandler);
            vTaskDelete(_interfaceTaskHandler);
            _interfaceTaskHandler = NULL;
            logi::p("iController", "Interface thread: STOP");
        }
    }

    /// @brief Запуск процесса отрисовки
    void init()
    {
        // Прикрепить процесс к ядру
        xTaskCreatePinnedToCore(
            _drawInterfaceThread,   // Function to implement the task
            "drawInterfaceThread",  // Name of the task
            4096,                   // Stack size in bytes
            this,                   // Task input parameter
            10,                     // Priority of the task
            &_interfaceTaskHandler, // Task handle.
            tskNO_AFFINITY          // Core where the task should run
        );
    }
};