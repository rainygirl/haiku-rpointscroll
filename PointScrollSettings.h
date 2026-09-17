#ifndef POINT_SCROLL_SETTINGS_H
#define POINT_SCROLL_SETTINGS_H

#include <SupportDefs.h>


struct PointScrollSettings {
	bool	enabled;
	bool	horizontal;
	bool	natural;
	bool	acceleration;
	int32	speed;
	int32	deadZone;

	PointScrollSettings();
};


status_t LoadPointScrollSettings(PointScrollSettings& settings);
status_t SavePointScrollSettings(const PointScrollSettings& settings);


#endif
