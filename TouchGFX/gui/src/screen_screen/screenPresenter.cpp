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
	if (val >= 0 && val <= 50)
	{
		view.UpdateTempView(val);
	}
}

void screenPresenter::UpdateHum(int val)
{
	if (val >= 0 && val <= 100)
	{
		view.UpdateHumView(val);
	}
}
