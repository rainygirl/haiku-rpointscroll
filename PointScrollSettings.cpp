#include "PointScrollSettings.h"

#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>


static const char* kSettingsFile = "RPointScroll_settings";


PointScrollSettings::PointScrollSettings()
	:
	enabled(true),
	horizontal(true),
	natural(false),
	acceleration(true),
	speed(5),
	deadZone(4)
{
}


static status_t
SettingsPath(BPath& path)
{
	status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	if (status != B_OK)
		return status;
	return path.Append(kSettingsFile);
}


status_t
LoadPointScrollSettings(PointScrollSettings& settings)
{
	BPath path;
	status_t status = SettingsPath(path);
	if (status != B_OK)
		return status;

	BFile file(path.Path(), B_READ_ONLY);
	status = file.InitCheck();
	if (status != B_OK)
		return status;

	BMessage message;
	status = message.Unflatten(&file);
	if (status != B_OK)
		return status;

	message.FindBool("enabled", &settings.enabled);
	message.FindBool("horizontal", &settings.horizontal);
	message.FindBool("natural", &settings.natural);
	message.FindBool("acceleration", &settings.acceleration);
	message.FindInt32("speed", &settings.speed);
	message.FindInt32("dead_zone", &settings.deadZone);

	if (settings.speed < 1)
		settings.speed = 1;
	else if (settings.speed > 10)
		settings.speed = 10;
	if (settings.deadZone < 0)
		settings.deadZone = 0;
	else if (settings.deadZone > 12)
		settings.deadZone = 12;

	return B_OK;
}


status_t
SavePointScrollSettings(const PointScrollSettings& settings)
{
	BPath path;
	status_t status = SettingsPath(path);
	if (status != B_OK)
		return status;

	BMessage message('RPSC');
	message.AddBool("enabled", settings.enabled);
	message.AddBool("horizontal", settings.horizontal);
	message.AddBool("natural", settings.natural);
	message.AddBool("acceleration", settings.acceleration);
	message.AddInt32("speed", settings.speed);
	message.AddInt32("dead_zone", settings.deadZone);

	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	status = file.InitCheck();
	if (status != B_OK)
		return status;
	return message.Flatten(&file);
}
