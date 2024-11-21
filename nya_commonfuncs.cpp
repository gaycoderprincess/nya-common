#include <format>
#include <string>
#include <fstream>

#include "nya_commonbase.h"

struct tNyaPadState {
	int32_t keys[NUM_NYA_PAD_KEYS];

	const static int gTriggerDeadzone = 16;
	const static int gStickDeadzone = 2048;
	const static int gStickAsButtonDeadzone = 16383;

	tNyaPadState() {
		memset(this, 0, sizeof(*this));
	}

	void Update(XINPUT_STATE& state) {
		keys[NYA_PAD_KEY_DPAD_UP] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
		keys[NYA_PAD_KEY_DPAD_DOWN] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
		keys[NYA_PAD_KEY_DPAD_LEFT] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
		keys[NYA_PAD_KEY_DPAD_RIGHT] = state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
		keys[NYA_PAD_KEY_START] = state.Gamepad.wButtons & XINPUT_GAMEPAD_START;
		keys[NYA_PAD_KEY_SELECT] = state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
		keys[NYA_PAD_KEY_L3] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
		keys[NYA_PAD_KEY_R3] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
		keys[NYA_PAD_KEY_LB] = state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
		keys[NYA_PAD_KEY_RB] = state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
		keys[NYA_PAD_KEY_A] = state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		keys[NYA_PAD_KEY_B] = state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
		keys[NYA_PAD_KEY_X] = state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
		keys[NYA_PAD_KEY_Y] = state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
		keys[NYA_PAD_KEY_LT] = state.Gamepad.bLeftTrigger > gTriggerDeadzone ? state.Gamepad.bLeftTrigger : 0;
		keys[NYA_PAD_KEY_RT] = state.Gamepad.bRightTrigger > gTriggerDeadzone ? state.Gamepad.bRightTrigger : 0;
		keys[NYA_PAD_KEY_LSTICK_X] = abs(state.Gamepad.sThumbLX) > gStickDeadzone ? state.Gamepad.sThumbLX : 0;
		keys[NYA_PAD_KEY_LSTICK_Y] = abs(state.Gamepad.sThumbLY) > gStickDeadzone ? state.Gamepad.sThumbLY : 0;
		keys[NYA_PAD_KEY_RSTICK_X] = abs(state.Gamepad.sThumbRX) > gStickDeadzone ? state.Gamepad.sThumbRX : 0;
		keys[NYA_PAD_KEY_RSTICK_Y] = abs(state.Gamepad.sThumbRY) > gStickDeadzone ? state.Gamepad.sThumbRY : 0;
		keys[NYA_PAD_KEY_LSTICK_X_NEG] = keys[NYA_PAD_KEY_LSTICK_X];
		keys[NYA_PAD_KEY_LSTICK_X_POS] = keys[NYA_PAD_KEY_LSTICK_X];
		keys[NYA_PAD_KEY_LSTICK_Y_NEG] = keys[NYA_PAD_KEY_LSTICK_Y];
		keys[NYA_PAD_KEY_LSTICK_Y_POS] = keys[NYA_PAD_KEY_LSTICK_Y];
		keys[NYA_PAD_KEY_RSTICK_X_NEG] = keys[NYA_PAD_KEY_RSTICK_X];
		keys[NYA_PAD_KEY_RSTICK_X_POS] = keys[NYA_PAD_KEY_RSTICK_X];
		keys[NYA_PAD_KEY_RSTICK_Y_NEG] = keys[NYA_PAD_KEY_RSTICK_Y];
		keys[NYA_PAD_KEY_RSTICK_Y_POS] = keys[NYA_PAD_KEY_RSTICK_Y];

		if (keys[NYA_PAD_KEY_LSTICK_X_NEG] > 0) keys[NYA_PAD_KEY_LSTICK_X_NEG] = 0;
		if (keys[NYA_PAD_KEY_LSTICK_Y_NEG] > 0) keys[NYA_PAD_KEY_LSTICK_Y_NEG] = 0;
		if (keys[NYA_PAD_KEY_RSTICK_X_NEG] > 0) keys[NYA_PAD_KEY_RSTICK_X_NEG] = 0;
		if (keys[NYA_PAD_KEY_RSTICK_Y_NEG] > 0) keys[NYA_PAD_KEY_RSTICK_Y_NEG] = 0;
		if (keys[NYA_PAD_KEY_LSTICK_X_POS] < 0) keys[NYA_PAD_KEY_LSTICK_X_POS] = 0;
		if (keys[NYA_PAD_KEY_LSTICK_Y_POS] < 0) keys[NYA_PAD_KEY_LSTICK_Y_POS] = 0;
		if (keys[NYA_PAD_KEY_RSTICK_X_POS] < 0) keys[NYA_PAD_KEY_RSTICK_X_POS] = 0;
		if (keys[NYA_PAD_KEY_RSTICK_Y_POS] < 0) keys[NYA_PAD_KEY_RSTICK_Y_POS] = 0;
	}

	int GetKey(int key, bool asButton) {
		int value = keys[key];
		if (key >= NYA_PAD_KEY_LSTICK_X_NEG && asButton && abs(value) < gStickAsButtonDeadzone) return 0;
		return value;
	}
};
tNyaPadState gPadState[XUSER_MAX_COUNT];
tNyaPadState gPadStateLast[XUSER_MAX_COUNT];

int GetPadKeyState(tNyaPadState* pads, int key, int padId, bool padKeyAsButton) {
	if (key <= NYA_PAD_KEY_NONE || key >= NUM_NYA_PAD_KEYS) return 0;
	if (padId < -1 || padId >= XUSER_MAX_COUNT) return 0;

	if (padId >= 0) return pads[padId].GetKey(key, padKeyAsButton);

	// get highest value of all pads if -1
	int value = 0;
	for (int i = 0; i < XUSER_MAX_COUNT; i++) {
		if (abs(pads[i].GetKey(key, padKeyAsButton)) > abs(value)) value = pads[i].GetKey(key, padKeyAsButton);
	}
	return value;
}

int GetPadKeyState(int key, int padId, bool padKeyAsButton) {
	return GetPadKeyState(gPadState, key, padId, padKeyAsButton);
}

int GetPadLastKeyState(int key, int padId, bool padKeyAsButton) {
	return GetPadKeyState(gPadStateLast, key, padId, padKeyAsButton);
}

bool IsPadKeyPressed(int key, int padId) {
	return abs(GetPadKeyState(key, padId, true)) > 0;
}

bool IsPadKeyJustPressed(int key, int padId) {
	return IsPadKeyPressed(key, padId) && abs(GetPadLastKeyState(key, padId, true)) == 0;
}

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

std::string GetTimeFromMilliseconds(int value, bool altFormat) {
	auto valueAsSeconds = value / 1000;
	int hours = valueAsSeconds / 3600;
	int minutes = valueAsSeconds / 60 % 60;
	int seconds = valueAsSeconds % 60;
	int milliseconds = value % 1000;

	const char* separator1 = altFormat ? "\'\'" : ".";
	const char* separator2 = altFormat ? "\'" : ":";

	if (hours != 0) {
		return std::format("{}{}{:02}{}{:02}{}{:03}", hours, separator2, minutes, separator2, seconds, separator1, milliseconds);
	} else {
		return std::format("{}{}{:02}{}{:03}", minutes, separator2, seconds, separator1, milliseconds);
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
	if (str.find(VK_RETURN) != std::string::npos) return false;
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

// XInputGetState without statically linking, from imgui :3
DWORD XInputGetState_Dynamic(int dwUserIndex, XINPUT_STATE* pState) {
	static auto funcPtr = (DWORD(WINAPI*)(DWORD, XINPUT_STATE*))nullptr;
	if (!funcPtr) {
		const char* dlls[] = {
				"xinput1_4.dll",
				"xinput1_3.dll",
				"xinput9_1_0.dll",
				"xinput1_2.dll",
				"xinput1_1.dll"
		};
		for (auto& file : dlls) {
			if (auto dll = LoadLibraryA(file)) {
				funcPtr = (DWORD(WINAPI*)(DWORD, XINPUT_STATE*))GetProcAddress(dll, "XInputGetState");
				break;
			}
		}
	}
	if (funcPtr) return funcPtr(dwUserIndex, pState);
	return 1;
}

DWORD XInputSetState_Dynamic(int dwUserIndex, XINPUT_VIBRATION* pState) {
	static auto funcPtr = (DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*))nullptr;
	if (!funcPtr) {
		const char* dlls[] = {
				"xinput1_4.dll",
				"xinput1_3.dll",
				"xinput9_1_0.dll",
				"xinput1_2.dll",
				"xinput1_1.dll"
		};
		for (auto& file : dlls) {
			if (auto dll = LoadLibraryA(file)) {
				funcPtr = (DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*))GetProcAddress(dll, "XInputSetState");
				break;
			}
		}
	}
	if (funcPtr) return funcPtr(dwUserIndex, pState);
	return 1;
}

void CommonMain(bool doDrawing) {
	if (doDrawing) NyaDrawing::DrawAll();

	if (!bDontRefreshInputsThisLoop) {
		memcpy(bKeyPressedLast, bKeyPressed, sizeof(bKeyPressed));

#ifndef NYA_INPUT_RUN_IN_BACKGROUND
		if (IsWindowInFocus()) {
#endif
			for (int i = 0; i < 255; i++) {
				bKeyPressed[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
			}

			for (int i = 0; i < XUSER_MAX_COUNT; i++) {
				XINPUT_STATE state;
				memset(&state, 0, sizeof(state));

				if (XInputGetState_Dynamic(i, &state) == ERROR_SUCCESS) {
					gPadStateLast[i] = gPadState[i];
					gPadState[i].Update(state);
				}
				else {
					memset(&gPadState[i], 0, sizeof(gPadState[i]));
				}
			}
#ifndef NYA_INPUT_RUN_IN_BACKGROUND
		} else {
			memset(bKeyPressed, 0, sizeof(bKeyPressed));
			memset(gPadState, 0, sizeof(gPadState));
		}
#endif
	}
	bDontRefreshInputsThisLoop = false;
}