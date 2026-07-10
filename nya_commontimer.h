#include <chrono>

class CNyaTimer {
public:
	std::chrono::steady_clock::time_point tLastUpdate;
	double fDeltaTime;
	double fTotalTime;
	double fMinDelta;
	uint64_t nDeltaTimeMicroseconds;

	CNyaTimer();
	CNyaTimer(float minDelta);
	double Process();
};