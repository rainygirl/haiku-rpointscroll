#include "../ScrollMath.h"

#include <assert.h>
#include <math.h>


int
main()
{
	assert(fabs(PointScrollScale(1) - 0.065f) < 0.0001f);
	assert(fabs(PointScrollScale(10) - 0.29f) < 0.0001f);
	assert(PointScrollScale(0) == PointScrollScale(1));
	assert(PointScrollScale(99) == PointScrollScale(10));

	assert(!PointScrollPastDeadZone(3, 0, 4, true));
	assert(PointScrollPastDeadZone(4, 0, 4, true));
	assert(!PointScrollPastDeadZone(12, 0, 4, false));
	assert(PointScrollPastDeadZone(12, 4, 4, false));

	assert(PointScrollAcceleration(20, 0, false) == 1.0f);
	assert(PointScrollAcceleration(24, 0, true) > 1.0f);
	assert(PointScrollAcceleration(100, 100, true) == 2.5f);
	return 0;
}
