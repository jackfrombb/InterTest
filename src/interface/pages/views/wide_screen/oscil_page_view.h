#include "interface/pages/views/page_view.h"

class OscilPageView : public PageView
{
public:
    ElWaveform<uint16_t> _waveform;
    ElText _waitText;

    ElText _test1;
    ElText _test2;
    ElCenteredGroup _bottomButtons;

    OscilPageView(DisplayVirtual *display) : PageView(display)
    {
        _initWaveform();
        _initWaitText();
        _initTest();

        addEllement(&_waveform)->addEllement(&_waitText)->addEllement(&_bottomButtons);
    }

    void _initTest()
    {
        _test1.setText("Назад")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setWidth(20)->setHeight(10);

        _test2.setText("Настройки")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setWidth(30)->setHeight(10);

        int16_t paddingW = 2;
        int16_t paddingBottom = 2;

        _bottomButtons
            .setX(paddingW)
            ->setY(_display->getHeight() - (_test1.getHeight() + 4 + paddingBottom))
            ->setWidth(_display->getWidth() - paddingW)
            ->setHeight(_test1.getHeight() + 4);

        _bottomButtons.addEllement(&_test1)->addEllement(&_test2);
    }

    void _initWaveform()
    {
        display_position size = {
            .leftUp{
                .x = 0,
                .y = 0,
            },
            .rightDown{
                .x = _display->getResoluton().width,
                .y = _display->getResoluton().height}};

        _waveform.setArea(size);
    }

    void _initWaitText()
    {
        _waitText.setText("Подождите")->setPosition(ELEMENT_POSITION_CENTER, ELEMENT_POSITION_CENTER)->setTextSize(EL_TEXT_SIZE_SMALL);
    }
};