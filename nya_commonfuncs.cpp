#include <format>
#include <string>
#include <fstream>

#include "nya_commonbase.h"

bool bKeyPressed[255];
bool bKeyPressedLast[255];
bool bDontRefreshInputsThisLoop = false;
bool IsKeyPressed(int key) {
	if (key <= 0) return false;
	if (key >= 255) return false;
	return bKeyPressed[key];
}
bool IsKeyJustPressed(int key) {
	if (key <= 0) return false;
	if (key >= 255) return false;
	return bKeyPressed[key] && !bKeyPressedLast[key];
}

std::string GetTimeFromSeconds(int value) {
	int hours = value / 3600;
	int minutes = value / 60 % 60;
	int seconds = value % 60;

	if (hours != 0) {
		return std::format("{}:{:02}:{:02}", hours, minutes, seconds);
	} else {
		return std::format("{}:{:02}", minutes, seconds);
	}
}

bool StringHasEnding (std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

std::string CapStringToWidth(const char* str, float size, float width) {
	std::string ret = str;

	while (ret.length() > 4 && GetStringWidth(size, ret.c_str()) > width) {
		ret.pop_back();
		ret.pop_back();
		ret.pop_back();
		ret.pop_back();
		ret += "...";
	}

	return ret;
}

bool IsWriteableInputCharacter(std::string& str) {
	if (str.find(VK_BACK) != std::string::npos) return false;
	if (str.find(VK_ESCAPE) != std::string::npos) return false;
	return true;
}

void WriteStringToFile(std::ofstream& file, const char* string) {
	int len  = lstrlen(string) + 1;
	file.write((char*)&len, sizeof(len));
	file.write(string, len);
}

std::string ReadStringFromFile(std::ifstream& file) {
	int len = 0;
	file.read((char*)&len, sizeof(len));
	if (len <= 0) return "";

	char* tmp = new char[len];
	file.read(tmp, len);
	std::string str = tmp;
	delete[] tmp;

	return str;
}

void CommonMain() {
	NyaDrawing::DrawAll();

	if (!bDontRefreshInputsThisLoop) {
		memcpy(bKeyPressedLast, bKeyPressed, sizeof(bKeyPressed));

		if (IsWindowInFocus()) {
			for (int i = 0; i < 255; i++) {
				bKeyPressed[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
			}
		} else {
			memset(bKeyPressed, 0, sizeof(bKeyPressed));
		}
	}
	bDontRefreshInputsThisLoop = false;
}