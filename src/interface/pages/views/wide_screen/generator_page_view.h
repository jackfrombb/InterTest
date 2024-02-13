#pragma once

class GeneratorPageView : public PageView
{
    ElText onOffTitleText;
    ElTextButton onButton;
    ElTextButton offButton;

public:
    GeneratorPageView(DisplayVirtual *display) : PageView(display)
    {

    }
};

/*
Заметки:
На странице нужно:
- Включать и выключать генерацию
- Управлять частотой и скважностью сигнала
В будущем можно будет изменять тип сигнала, но для начала достаточно меандра

Состояние: Вкл. Выкл
Частота: 2000 hz (Подчеркнуто при вводе)
Скважность: 50% (меняется при )
*/