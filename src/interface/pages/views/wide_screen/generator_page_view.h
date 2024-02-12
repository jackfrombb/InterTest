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
- Выбирать тип сигнала (На данный момент это меандр, можно еще полуволну или волну в положительном диапазоне, но смысла в этом нет)
- Управлять частотой и скважностью сигнала
*/