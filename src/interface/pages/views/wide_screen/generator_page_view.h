#pragma once

class GeneratorPageView : public PageView
{
    ElRecycleGroup recycleGroup;

    function<ElGroup *(uint16_t position)> _elementGetter = [this](uint16_t position)
    {
        ElGroup *group;
        switch (position)
        {
        case 0:
            group = _getOnOffPage();
            break;

        case 1:
            break;
        case 2:
            break;

        default:
            break;
        }

        return group;
    };
    uint8_t buttonFocus = 0;
    ElGroup *_getOnOffPage()
    {

        ElGroup *root = new ElGroup();
        ElText *onOffTitleText = new ElText();
        onOffTitleText
            ->setText("Состояние:")
            ->utf8Patch()
            ->setX(0)
            ->setY(0);

        ElCenteredGroup *toggleGroup = new ElCenteredGroup();
        toggleGroup
            ->setNeedDrawFrame(true)
            ->setX(0)
            ->setWidth(_display->getWidth())
            ->setHeight(_display->getMaxTextHeight(el_text_size::EL_TEXT_SIZE_MIDDLE))
            ->setY((_display->getHeight() >> 1) - (toggleGroup->getHeight() >> 1))
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        ElTextButton *onButton = new ElTextButton();
        onButton->setButtonId(0)
            ->setSelectedButtonPtr(&buttonFocus)
            ->setText("Вкл")
            ->utf8Patch()
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        ElTextButton *offButton = new ElTextButton();
        offButton->setButtonId(1)
            ->setSelectedButtonPtr(&buttonFocus)
            ->setText("Выкл")
            ->utf8Patch()
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        toggleGroup->addElement(onButton)->addElement(offButton);

        root->addElement(onOffTitleText)->addElement(toggleGroup);

        return root;
    }

public:
    GeneratorPageView(DisplayVirtual *display) : PageView(display)
    {
        recycleGroup.setElementsSingleSize(area_size{.width = display->getWidth(), .height = display->getHeight()});
        recycleGroup.setElementGetter(_elementGetter, 3);
        addElement(&recycleGroup);
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