#include <gui/screen_screen/screenView.hpp>
#include <string.h>
#include <string>

screenView::screenView()
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();

	tempTextViewBuffer[0] = 0;
	humTextViewBuffer[0] = 0;
	//screenViewBase::tempText.setPosition(156, 71, 240, 320);

	screenViewBase::tempText.setPosition(100, 71, 240, 320);
	screenViewBase::tempText.setWildcard(tempTextViewBuffer);

    screenViewBase::humText.setPosition(100, 180, 240, 320);
    screenViewBase::humText.setWildcard(humTextViewBuffer);

}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::HumidifierControlButtonState()
{
	//presenter->updateHumidifierControlState(buttonOnOff.getState());
}

void screenView::updateOnOffButtonState()
{
	presenter->updateHumidifierControlState(humOnOffButton.getState());
}

void screenView::updateOnButton()
{
	 screenViewBase::humOnOffButton.forceState(true);
}

void screenView::updateOffButton()
{
	screenViewBase::humOnOffButton.forceState(false);
}

void screenView::UpdateTempView(int val)
{
	std::string s = std::to_string(val);
	char const *pchar = s.c_str();  //use char const* as target type

	//memset(tempTextViewBuffer, 0x0, sizeof(tempTextViewBuffer));

	Unicode::strncpy(tempTextViewBuffer, pchar, strlen(pchar));
	screenViewBase::tempText.invalidate();
}

void screenView::UpdateHumView(int val)
{
	std::string s = std::to_string(val);

	s += "%";
	char const *pchar = s.c_str();  //use char const* as target type
	Unicode::strncpy(humTextViewBuffer, pchar, strlen(pchar));
	screenViewBase::humText.invalidate();
}

