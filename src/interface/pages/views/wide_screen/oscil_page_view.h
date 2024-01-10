#include "interface/pages/views/page_view.h"

class OscilPageView : public PageView
{
public:
    ElWaveform<uint16_t> _waveform;
    ElText _waitText;

    OscilPageView(DisplayVirtual* display): PageView(display)
    {
        _initWaveform();
        _initWaitText();
        
        _ellements = {
            &_waveform,
            &_waitText,
        };
    }

    void _initWaveform()
    {
        display_area size = {
            .leftUp{
                .x = 0,
                .y = 0,
            },
            .rightDown{.x = _display->getResoluton().width, .y = _display->getResoluton().height}};
        _waveform.setArea(size);
    }

    void _initWaitText()
    {
        _waitText.setTextSize(EL_TEXT_SIZE_SMALL);
        _waitText.setText("Подождите");
        _waitText.setPosition(-1, -1);
    }

};