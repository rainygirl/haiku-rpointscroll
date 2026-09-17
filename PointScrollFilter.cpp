#include "PointScrollSettings.h"
#include "ScrollMath.h"

#include <new>

#include <InterfaceDefs.h>
#include <List.h>
#include <Message.h>
#include <OS.h>
#include <View.h>
#include <add-ons/input_server/InputServerDevice.h>
#include <add-ons/input_server/InputServerFilter.h>


extern "C" _EXPORT BInputServerFilter* instantiate_input_filter();


class PointScrollFilter : public BInputServerFilter {
public:
	PointScrollFilter();
	virtual ~PointScrollFilter();
	virtual filter_result Filter(BMessage* message, BList* outList);

private:
	void _ReloadSettings();
	filter_result _MouseDown(BMessage* message);
	filter_result _MouseMoved(BMessage* message);
	filter_result _MouseUp(BMessage* message, BList* outList);
	void _ResetGesture();

	PointScrollSettings fSettings;
	bigtime_t fLastSettingsCheck;
	BMessage* fMiddleDown;
	bool fPending;
	bool fScrolling;
	int32 fAccumulatedX;
	int32 fAccumulatedY;
};


PointScrollFilter::PointScrollFilter()
	:
	fLastSettingsCheck(0),
	fMiddleDown(NULL),
	fPending(false),
	fScrolling(false),
	fAccumulatedX(0),
	fAccumulatedY(0)
{
	LoadPointScrollSettings(fSettings);
}


PointScrollFilter::~PointScrollFilter()
{
	delete fMiddleDown;
}


void
PointScrollFilter::_ReloadSettings()
{
	// Never change policy halfway through a held-button gesture.
	if (fPending)
		return;

	bigtime_t now = system_time();
	if (now - fLastSettingsCheck < 1000000)
		return;

	fLastSettingsCheck = now;
	PointScrollSettings settings;
	if (LoadPointScrollSettings(settings) == B_OK)
		fSettings = settings;
}


void
PointScrollFilter::_ResetGesture()
{
	delete fMiddleDown;
	fMiddleDown = NULL;
	fPending = false;
	fScrolling = false;
	fAccumulatedX = 0;
	fAccumulatedY = 0;
}


filter_result
PointScrollFilter::_MouseDown(BMessage* message)
{
	int32 buttons = 0;
	int32 subtype = B_MOUSE_POINTING_DEVICE;
	message->FindInt32("be:device_subtype", &subtype);
	if (message->FindInt32("buttons", &buttons) != B_OK
		|| subtype != B_MOUSE_POINTING_DEVICE
		|| (buttons & B_TERTIARY_MOUSE_BUTTON) == 0
		|| (buttons & (B_PRIMARY_MOUSE_BUTTON | B_SECONDARY_MOUSE_BUTTON)) != 0) {
		return B_DISPATCH_MESSAGE;
	}

	_ResetGesture();
	fMiddleDown = new(std::nothrow) BMessage(*message);
	if (fMiddleDown == NULL)
		return B_DISPATCH_MESSAGE;

	fPending = true;
	return B_SKIP_MESSAGE;
}


filter_result
PointScrollFilter::_MouseMoved(BMessage* message)
{
	if (!fPending)
		return B_DISPATCH_MESSAGE;

	int32 buttons = 0;
	int32 x = 0;
	int32 y = 0;
	message->FindInt32("buttons", &buttons);
	message->FindInt32("x", &x);
	message->FindInt32("y", &y);

	if ((buttons & B_TERTIARY_MOUSE_BUTTON) == 0) {
		_ResetGesture();
		return B_DISPATCH_MESSAGE;
	}

	fAccumulatedX += x;
	fAccumulatedY += y;
	if (!fScrolling) {
		if (!PointScrollPastDeadZone(fAccumulatedX, fAccumulatedY,
				fSettings.deadZone, fSettings.horizontal)) {
			return B_SKIP_MESSAGE;
		}
		fScrolling = true;
		x = fAccumulatedX;
		y = fAccumulatedY;
		delete fMiddleDown;
		fMiddleDown = NULL;
	}

	float scale = PointScrollScale(fSettings.speed);
	float factor = PointScrollAcceleration(x, y, fSettings.acceleration);
	float direction = fSettings.natural ? -1.0f : 1.0f;
	float wheelX = fSettings.horizontal ? x * scale * factor * direction : 0.0f;
	float wheelY = y * scale * factor * direction;

	message->what = B_MOUSE_WHEEL_CHANGED;
	message->RemoveName("buttons");
	message->RemoveName("x");
	message->RemoveName("y");
	message->RemoveName("clicks");
	message->AddFloat("be:wheel_delta_x", wheelX);
	message->AddFloat("be:wheel_delta_y", wheelY);
	return B_DISPATCH_MESSAGE;
}


filter_result
PointScrollFilter::_MouseUp(BMessage* message, BList* outList)
{
	if (!fPending)
		return B_DISPATCH_MESSAGE;

	int32 buttons = 0;
	message->FindInt32("buttons", &buttons);
	if ((buttons & B_TERTIARY_MOUSE_BUTTON) != 0)
		return B_DISPATCH_MESSAGE;

	if (fScrolling) {
		_ResetGesture();
		return B_SKIP_MESSAGE;
	}

	BMessage* down = fMiddleDown;
	fMiddleDown = NULL;
	BMessage* up = new(std::nothrow) BMessage(*message);
	_ResetGesture();

	if (down == NULL || up == NULL) {
		delete down;
		delete up;
		return B_DISPATCH_MESSAGE;
	}

	if (!outList->AddItem(down) || !outList->AddItem(up)) {
		outList->RemoveItem(down);
		outList->RemoveItem(up);
		delete down;
		delete up;
		return B_DISPATCH_MESSAGE;
	}
	return B_DISPATCH_MESSAGE;
}


filter_result
PointScrollFilter::Filter(BMessage* message, BList* outList)
{
	_ReloadSettings();
	if (!fSettings.enabled) {
		if (fPending)
			_ResetGesture();
		return B_DISPATCH_MESSAGE;
	}

	switch (message->what) {
		case B_MOUSE_DOWN:
			return _MouseDown(message);
		case B_MOUSE_MOVED:
			return _MouseMoved(message);
		case B_MOUSE_UP:
			return _MouseUp(message, outList);
		default:
			return B_DISPATCH_MESSAGE;
	}
}


BInputServerFilter*
instantiate_input_filter()
{
	return new(std::nothrow) PointScrollFilter();
}
