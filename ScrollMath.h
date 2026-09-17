#ifndef POINT_SCROLL_MATH_H
#define POINT_SCROLL_MATH_H

#include <math.h>


inline float
PointScrollScale(int speed)
{
	if (speed < 1)
		speed = 1;
	else if (speed > 10)
		speed = 10;
	return 0.04f + speed * 0.025f;
}


inline float
PointScrollAcceleration(int x, int y, bool enabled)
{
	if (!enabled)
		return 1.0f;

	float distance = (float)sqrt((double)x * x + (double)y * y);
	float factor = 1.0f + distance / 16.0f;
	return factor > 2.5f ? 2.5f : factor;
}


inline bool
PointScrollPastDeadZone(int x, int y, int deadZone, bool horizontal)
{
	if (!horizontal)
		x = 0;
	return x * x + y * y >= deadZone * deadZone;
}


#endif
