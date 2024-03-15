#include "nya_commontimer.h"

void CNyaTimer::Process()
{
	auto now = std::chrono::steady_clock::now();
	fDeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - tLastUpdate).count() / 1000000.0f;
	tLastUpdate = now;

	if (fDeltaTime > fMinDelta) fDeltaTime = fMinDelta;
}