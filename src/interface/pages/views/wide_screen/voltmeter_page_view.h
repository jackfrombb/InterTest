#pragma once

// #include "interface/pages/views/page_view.h"
// #include "displays/display_virtual.h"

class VoltemeterPageView : public PageView
{
private:
    String _voltTextValue;

    uint8_t _mainVlotType = 0; // 0 - maxVolt, 1- midVolt

    ElText _titleForBigCenterText;
    ElText _bigCenterText;

    ElWaveform _waveform;

    ElCenteredGroup _downInfoGroup;
    ElText _smallLeftText;
    ElText _smallRightText;

public:
    VoltemeterPageView(DisplayVirtual *display, InterfaceEngineVirtual *iEngine) : PageView(display)
    {
        setMainVoltType(0);

        // Общий контейнер для полей по низу экрана
        _downInfoGroup
            .setNeedDrawFrame(false)
            ->addElement(&_smallLeftText)
            ->addElement(&_smallRightText)
            ->setWidth(display->getWidth())
            ->setHeight(iEngine->getMaxTextHeight(el_text_size::EL_TEXT_SIZE_SMALL))
            ->setY(_display->getHeight() - _downInfoGroup.getHeight());

        // Инциализируем осциллограмму вверху экрана
        _waveform
            .setNeedDrawBackground(false)
            ->setX(0)
            ->setWidth(display->getWidth())
            ->setHeight(display->getHeight() - ((display->getHeight() >> 1) + 10)); // Из высоты экрана удаляем половину высоты экрана + чуть больше половины высоты текста

        // Тип основного измерения
        _titleForBigCenterText
            .setX(0)
            ->setY(_waveform.getHeight() + // отступаем на высоту осцила и выставляем по середине текста справа
                   ((_display->getMaxTextHeight(el_text_size::EL_VOLTMETER_VALUE_LARGE) >> 1) - (_display->getMaxTextHeight(_titleForBigCenterText.getTextSize()) >> 1)));
        //->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        // Инициализируем основное поле по центру экрана
        _bigCenterText
            .setText("_.__")
            ->setTextSize(el_text_size::EL_VOLTMETER_VALUE_LARGE)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH)
            ->setX(_display->getMaxTextWidth(_titleForBigCenterText.getTextSize()) * _titleForBigCenterText.getText().length()) // Отступаем надпись max по x
            ->setY(_waveform.getHeight())
            ->setWidth(_display->getWidth() - _bigCenterText.getX()) // Занимаем остальное место после x

            //->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ;

        // Второстепенные поля по низу экрана и по середине своей ширины (ширину устновит контейнер)
        _smallLeftText.setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH);
        _smallRightText.setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER_SELF_WIDTH);

        addElement(&_waveform)
            ->addElement(&_titleForBigCenterText)
            ->addElement(&_bigCenterText)
            ->addElement(&_downInfoGroup);
    }

    String *getSmallLeftTextPtr()
    {
        return _smallLeftText.getTextPtr();
    }

    String *getSampleRateTextPtr()
    {
        return _smallRightText.getTextPtr();
    }

    /// @brief Получить ссылку на основное поле вольтажа
    String *getBigCenterTextPattern()
    {
        return _bigCenterText.getTextPtr();
    }

    /// @brief Получть ссылку для управления формой сигнала
    adc_measures_t *getWaveformMeasuresPtr()
    {
        return _waveform.getMeasuresPtr();
    }

    // 0 - maxVolt, 1- midVolt
    void setMainVoltType(const uint8_t type)
    {
        _mainVlotType = type;
        switch (type)
        {
        case 0:
        default:
            _titleForBigCenterText.setText("max");
            break;

        case 1:
            _titleForBigCenterText.setText("mid:");
            break;
        }
    }

    bool onControlEvent(control_event_type eventType) override
    {
        return false;
    }
};