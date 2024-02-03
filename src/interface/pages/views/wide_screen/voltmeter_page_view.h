#pragma once

//#include "interface/pages/views/page_view.h"
//#include "displays/display_virtual.h"

class VoltemeterPageView : public PageView
{
private:
    String _voltTextValue;

    uint8_t _mainVlotType = 0; // 0 - maxVolt, 1- midVolt

    ElText _mainVoltTitleText;
    ElText _voltText;

    ElWaveform _waveform;

    ElCenteredGroup _downInfoGroup;
    ElText _secondVoltText;
    ElText _sampleRateText;

public:
    VoltemeterPageView(DisplayVirtual *display, InterfaceEngineVirtual *iEngine) : PageView(display)
    {
        setMainVoltType(0);

        // Инциализируем осциллограмму вверху экрана
        _waveform
            .setNeedDrawBackground(false)
            ->setX(0)
            ->setWidth(display->getWidth())
            ->setHeight(display->getHeight() - ((display->getHeight() >> 1) + 10)); // Из высоты экрана удаляем половину высоты экрана + чуть больше половины высоты текста

        // Тип основного измерения
        _mainVoltTitleText
            .setX(0)
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        // Инициализируем основное поле по центру экрана
        _voltText
            .setText("_.__")
            ->setTextSize(el_text_size::EL_VOLTMETER_VALUE_LARGE)
            ->setX(iEngine->getMaxTextWidth(el_text_size::EL_TEXT_SIZE_MIDDLE) * 4) // Отступаем на 4 символа
            ->setVerticalAlignment(el_vertical_align::EL_ALIGN_CENTER);

        // Второстепенные поля по низу экрана и по середине своей ширины (ширину устновит контейнер)
        _secondVoltText.setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER);
        _sampleRateText.setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER);

        // Общий контейнер для полей по низу экрана
        _downInfoGroup
            .setNeedDrawFrame(false)
            ->addElement(&_secondVoltText)
            ->addElement(&_sampleRateText)
            ->setWidth(display->getWidth())
            ->setHeight(iEngine->getMaxTextHeight(el_text_size::EL_TEXT_SIZE_SMALL))
            ->setY(_display->getHeight() - _downInfoGroup.getHeight());

        addElement(&_waveform)
            ->addElement(&_mainVoltTitleText)
            ->addElement(&_voltText)
            ->addElement(&_downInfoGroup);
    }

    String *getSecondVoltTextPtr()
    {
        return _secondVoltText.getTextPtr();
    }

    String *getSampleRateTextPtr()
    {
        return _sampleRateText.getTextPtr();
    }

    /// @brief Получить ссылку на основное поле вольтажа
    String *getVoltPattern()
    {
        return _voltText.getTextPtr();
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
            _mainVoltTitleText.setText("max");
            break;

        case 1:
            _mainVoltTitleText.setText("mid:");
            break;
        }
    }

    bool onControlEvent(control_event_type eventType) override
    {
        return false;
    }
};