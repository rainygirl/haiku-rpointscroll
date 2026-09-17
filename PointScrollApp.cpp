#include "PointScrollSettings.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <Slider.h>
#include <StringView.h>
#include <Window.h>


enum {
	kSettingChanged = 'PSCH',
	kDefaults = 'PSDF'
};


class SettingsWindow : public BWindow {
public:
	SettingsWindow();
	virtual void MessageReceived(BMessage* message);
	virtual bool QuitRequested();

private:
	void _Show(const PointScrollSettings& settings);
	void _Save();

	BCheckBox* fEnabled;
	BCheckBox* fHorizontal;
	BCheckBox* fNatural;
	BCheckBox* fAcceleration;
	BSlider* fSpeed;
	BSlider* fDeadZone;
	BStringView* fStatus;
};


SettingsWindow::SettingsWindow()
	:
	BWindow(BRect(100, 100, 570, 410), "R PointScroll",
		B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	BView* background = new BView(Bounds(), "background", B_FOLLOW_ALL,
		B_WILL_DRAW);
	background->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	AddChild(background);

	fEnabled = new BCheckBox(BRect(20, 18, 450, 40), "enabled",
		"Enable middle-button scrolling", new BMessage(kSettingChanged));
	fHorizontal = new BCheckBox(BRect(20, 48, 450, 70), "horizontal",
		"Allow horizontal scrolling", new BMessage(kSettingChanged));
	fNatural = new BCheckBox(BRect(20, 78, 450, 100), "natural",
		"Natural direction (move content with the stick)",
		new BMessage(kSettingChanged));
	fAcceleration = new BCheckBox(BRect(20, 108, 450, 130), "acceleration",
		"Accelerate faster stick movements", new BMessage(kSettingChanged));

	fSpeed = new BSlider(BRect(20, 145, 450, 195), "speed", "Scroll speed",
		new BMessage(kSettingChanged), 1, 10, B_HORIZONTAL);
	fSpeed->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fSpeed->SetHashMarkCount(10);
	fSpeed->SetLimitLabels("Slow", "Fast");

	fDeadZone = new BSlider(BRect(20, 210, 450, 260), "dead_zone",
		"Movement before scrolling starts", new BMessage(kSettingChanged),
		0, 12, B_HORIZONTAL);
	fDeadZone->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fDeadZone->SetHashMarkCount(7);
	fDeadZone->SetLimitLabels("None", "12 px");

	BButton* defaults = new BButton(BRect(20, 270, 150, 298), "defaults",
		"Restore defaults", new BMessage(kDefaults));
	fStatus = new BStringView(BRect(170, 273, 450, 297), "status", "");

	background->AddChild(fEnabled);
	background->AddChild(fHorizontal);
	background->AddChild(fNatural);
	background->AddChild(fAcceleration);
	background->AddChild(fSpeed);
	background->AddChild(fDeadZone);
	background->AddChild(defaults);
	background->AddChild(fStatus);

	PointScrollSettings settings;
	LoadPointScrollSettings(settings);
	_Show(settings);
}


void
SettingsWindow::_Show(const PointScrollSettings& settings)
{
	fEnabled->SetValue(settings.enabled ? B_CONTROL_ON : B_CONTROL_OFF);
	fHorizontal->SetValue(settings.horizontal ? B_CONTROL_ON : B_CONTROL_OFF);
	fNatural->SetValue(settings.natural ? B_CONTROL_ON : B_CONTROL_OFF);
	fAcceleration->SetValue(settings.acceleration ? B_CONTROL_ON : B_CONTROL_OFF);
	fSpeed->SetValue(settings.speed);
	fDeadZone->SetValue(settings.deadZone);
}


void
SettingsWindow::_Save()
{
	PointScrollSettings settings;
	settings.enabled = fEnabled->Value() == B_CONTROL_ON;
	settings.horizontal = fHorizontal->Value() == B_CONTROL_ON;
	settings.natural = fNatural->Value() == B_CONTROL_ON;
	settings.acceleration = fAcceleration->Value() == B_CONTROL_ON;
	settings.speed = fSpeed->Value();
	settings.deadZone = fDeadZone->Value();

	status_t status = SavePointScrollSettings(settings);
	fStatus->SetText(status == B_OK ? "Saved — applies within one second"
		: "Could not save settings");
}


void
SettingsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kSettingChanged:
			_Save();
			break;
		case kDefaults:
		{
			PointScrollSettings settings;
			_Show(settings);
			_Save();
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}


bool
SettingsWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


class PointScrollApp : public BApplication {
public:
	PointScrollApp()
		:
		BApplication("application/x-vnd.rainygirl-rpointscroll")
	{
	}

	virtual void ReadyToRun()
	{
		SettingsWindow* window = new SettingsWindow();
		window->Show();
	}
};


int
main()
{
	PointScrollApp app;
	app.Run();
	return 0;
}
