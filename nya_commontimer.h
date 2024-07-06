#include <chrono>

class CNyaTimer {
public:
	std::chrono::steady_clock::time_point tLastUpdate;
	double fDeltaTime;
	double fMinDelta;

	CNyaTimer();
	CNyaTimer(float minDelta);
	double Process();
};