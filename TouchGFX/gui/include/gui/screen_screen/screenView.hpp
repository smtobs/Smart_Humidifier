#ifndef SCREENVIEW_HPP
#define SCREENVIEW_HPP

#include <gui_generated/screen_screen/screenViewBase.hpp>
#include <gui/screen_screen/screenPresenter.hpp>

class screenView : public screenViewBase
{
public:
    screenView();
    virtual ~screenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void HumidifierControlButtonState();
    virtual void updateOnOffButtonState();
    virtual void updateOnButton();
    virtual void updateOffButton();
    virtual void UpdateTempView(int);
    virtual void UpdateHumView(int);
protected:
    static const uint16_t TEXTVIEW_BUFF_SIZE = 32;
    Unicode::UnicodeChar tempTextViewBuffer[TEXTVIEW_BUFF_SIZE];
    Unicode::UnicodeChar humTextViewBuffer[TEXTVIEW_BUFF_SIZE];
};

#endif // SCREENVIEW_HPP
