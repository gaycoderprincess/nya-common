#include <string>

namespace UpdateChecker {
	extern bool bUpdateChecked;
	extern bool bUpdateAvailable;
	extern std::string sUpdateVersion;
	void OpenUpdatePage();
	void CheckForUpdates();
}