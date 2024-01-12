#include "interface/pages/views/page_view.h"

class OscilPageView : public PageView
{
private:
    uint16_t _showBottomMenuFrames = 0;
    ElCenteredGroup _bottomButtons;

    display_position *_bottomMenuPosition;
    bool _showBottomMenu = true;
    int _bottomMenuPositionOnShow = 0;
    int16_t _bottomMenuHeight;

    ulong lastButtonPressTime;

    void _initBottomMenu()
    {
        _volt.setText("Vm")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVertialAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(20)
            ->setHeight(10);

        _herz.setText("Hz")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVertialAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(30)
            ->setHeight(10);

        _pause.setText("P")
            ->setTextSize(el_text_size::EL_TEXT_SIZE_SMALL)
            ->setAlignment(el_text_align::EL_TEXT_ALIGN_CENTER)
            ->setVertialAlignment(el_vertical_align::EL_ALIGN_CENTER)
            ->setWidth(30)
            ->setHeight(10);

        int16_t paddingW = 2;
        int16_t paddingBottom = 2;

        _bottomButtons
            .setX(paddingW)
            ->setY(_display->getHeight() - (_volt.getHeight() + 4 + paddingBottom))
            ->setWidth(_display->getWidth() - paddingW)
            ->setHeight(_volt.getHeight() + 4);

        _bottomButtons.addEllement(&_volt)->addEllement(&_herz)->addEllement(&_pause);
        _bottomMenuPosition = _bottomButtons.getAreaPtr();
        _bottomMenuPositionOnShow = _bottomButtons.getArea().getY();
        _bottomMenuHeight = _bottomButtons.getHeight();
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

public:
    ElWaveform<uint16_t> _waveform;
    ElText _waitText;

    ElText _volt;
    ElText _herz;
    ElText _pause;

    OscilPageView(DisplayVirtual *display) : PageView(display)
    {
        _initWaveform();
        _initWaitText();
        _initBottomMenu();

        addEllement(&_waveform)->addEllement(&_waitText)->addEllement(&_bottomButtons);
        lastButtonPressTime = millis();
    }

    void onControlEvent(control_event_type eventType)
    {
        lastButtonPressTime = millis();
        _showBottomMenu = true;
    }

    void onDraw(int onMillis)
    {
        if (_showBottomMenu && _bottomMenuPosition->getY() > _bottomMenuPositionOnShow)
        {
            if (!_bottomButtons.isVisible())
            {
                _bottomButtons.setVisibility(true);
            }

            _bottomMenuPosition->leftUp.y -= 2;
            _bottomMenuPosition->rightDown.y -= 2;
        }
        else if (!_showBottomMenu && _bottomMenuPosition->getY() < _display->getResoluton().height)
        {
            _bottomMenuPosition->leftUp.y += 2;
            _bottomMenuPosition->rightDown.y += 2;

            if (_display->getResoluton().height - (_bottomButtons.getHeight() * 0.2) == _bottomMenuPosition->leftUp.y)
            {
                _bottomButtons.setVisibility(false);
            }
        }

        if (_showBottomMenu &&  millis() - lastButtonPressTime > 3000)
        {
            _showBottomMenu = false;
        }
    }
};