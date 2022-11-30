#include <gui/screen_screen/screenView.hpp>
#include <gui/screen_screen/screenPresenter.hpp>

screenPresenter::screenPresenter(screenView& v)
    : view(v)
{

}

void screenPresenter::activate()
{

}

void screenPresenter::deactivate()
{

}

void screenPresenter::updateHumidifierControlState(bool state)
{
	model->updateHumidifierControlState(state);
}

void screenPresenter::UpdateTemp(int val)
{
	if (val >= -50 && val <= 50)
	{
		view.UpdateTempView(val);
	}
}

void screenPresenter::UpdateHum(int val)
{
	if (val >= -50 && val <= 100)
	{
		view.UpdateHumView(val);
	}
}

bool prvStatus = false;

void screenPresenter::ChkUpdateOnOffButton(bool status)
{
	if (prvStatus == status)
	{
		return;
	}

	if (status == true)
	{
		view.updateOnButton();
	}
	else
	{
		view.updateOffButton();
	}
	prvStatus = status;
}
