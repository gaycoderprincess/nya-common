#include "nya_commontimer.h"

CNyaTimer::CNyaTimer() {
	fDeltaTime = 0;
	fMinDelta = 1.0 / 10.0; // delta for 10fps by default, any less and it's capped
}
CNyaTimer::CNyaTimer(float minDelta) {
	fDeltaTime = 0;
	fMinDelta = minDelta;
}
double CNyaTimer::Process() {
	auto now = std::chrono::steady_clock::now();
	nDeltaTimeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - tLastUpdate).count();
	fDeltaTime = nDeltaTimeMicroseconds / 1000000.0;
	tLastUpdate = now;

	if (fDeltaTime > fMinDelta) fDeltaTime = fMinDelta;
	fTotalTime += fDeltaTime;
	return fDeltaTime;
}