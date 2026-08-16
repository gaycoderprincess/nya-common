#include <filesystem>
#include <format>
#include <fstream>
#include <thread>

#include "curl/curl.h"
#include "nya_updatecheck.h"

namespace UpdateChecker {
	bool bUpdateChecked = false;
	bool bUpdateAvailable = false;
	std::string sUpdateVersion;

	std::string sProjectName = CWOEE_UPDATER_PROJECT;
	void OpenUpdatePage() {
#ifdef CWOEE_UPDATER_GIT_GAY
		ShellExecuteA(nullptr, "open", std::format("https://git.gay/gaycoderprincess/{}/releases/latest", sProjectName).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif CWOEE_UPDATER_GITHUB
		ShellExecuteA(nullptr, "open", std::format("https://github.com/gaycoderprincess/{}/releases/latest", sProjectName).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#endif
	}

	std::string GetFileName() {
		return std::filesystem::temp_directory_path().string() + "cwoee_releases.json";
	}

	size_t write_cb(char *ptr, size_t size, size_t nmemb, void *stream) {
		size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
		return written;
	}

	void CheckVersion(const std::string& versionStr) {
		if (versionStr[0] < '0' || versionStr[0] > '9') return;

		auto currVersion = std::stof(CWOEECHAOS_VERSION);
		auto newVersion = std::stof(versionStr);
		bUpdateAvailable = newVersion > currVersion;
		sUpdateVersion = versionStr;
		bUpdateChecked = true;
	}

	// "tag_name": "v1.16",
	void CheckVersion() {
		if (!std::filesystem::exists(GetFileName())) return;

		auto file = std::ifstream(GetFileName());
		for (std::string line; std::getline(file, line); ) {
			while (!line.empty() && !line.starts_with("\"tag_name\"")) {
				line.erase(line.begin());
			}
			if (line.empty()) continue;

			while (!line.empty() && line[0] != 'v') {
				line.erase(line.begin());
			}
			if (line.empty()) continue;
			line.erase(line.begin()); // so it's only the version number

			// cut off the end so the string remains as only 1.16, no quotes
			int endId = 0;
			while (endId < line.length() && line[endId] != '\"') {
				endId++;
			}
			while (line.length() > endId) {
				line.pop_back();
			}
			CheckVersion(line);
		}
		file.close();
	}

	void RetrieveVersionInfo() {
		auto result = curl_global_init(CURL_GLOBAL_ALL);
		if (result != CURLE_OK) {
			//WriteLog("Could not init curl");
			return;
		}

		if (auto curl = curl_easy_init()) {
#ifdef CWOEE_UPDATER_GIT_GAY
			curl_easy_setopt(curl, CURLOPT_URL, std::format("https://git.gay/api/v1/repos/gaycoderprincess/{}/releases", sProjectName).c_str());
#elif CWOEE_UPDATER_GITHUB
			curl_easy_setopt(curl, CURLOPT_URL, std::format("https://api.github.com/repos/gaycoderprincess/{}/releases", sProjectName).c_str());
#endif
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "CwoeeUpdateChecker");

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

			if (auto pagefile = fopen(GetFileName().c_str(), "wb")) {
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
				result = curl_easy_perform(curl);
				fclose(pagefile);

				CheckVersion();

				std::filesystem::remove(GetFileName());
			}

			curl_easy_cleanup(curl);
		}

		curl_global_cleanup();
	}

	void CheckForUpdates() {
		std::thread(RetrieveVersionInfo).detach();
	}
}