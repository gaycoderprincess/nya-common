#include <chrono>

class CNyaTimer {
public:
	std::chrono::steady_clock::time_point tLastUpdate;
	double fDeltaTime = 0;
	double fMinDelta = 1.0 / 10.0; // delta for 10fps by default, any less and it's capped

	void Process();
};