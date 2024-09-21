#include <string>
#include <cmath>
#include <xinput.h>

enum eNyaPadKey {
	NYA_PAD_KEY_NONE,
	NYA_PAD_KEY_DPAD_UP,
	NYA_PAD_KEY_DPAD_DOWN,
	NYA_PAD_KEY_DPAD_LEFT,
	NYA_PAD_KEY_DPAD_RIGHT,
	NYA_PAD_KEY_START,
	NYA_PAD_KEY_SELECT,
	NYA_PAD_KEY_L3,
	NYA_PAD_KEY_R3,
	NYA_PAD_KEY_LB,
	NYA_PAD_KEY_RB,
	NYA_PAD_KEY_A,
	NYA_PAD_KEY_B,
	NYA_PAD_KEY_X,
	NYA_PAD_KEY_Y,
	NYA_PAD_KEY_LT, // 0 - 255
	NYA_PAD_KEY_RT, // 0 - 255
	NYA_PAD_KEY_LSTICK_X_NEG, // -32768 - 32767
	NYA_PAD_KEY_LSTICK_X_POS, // -32768 - 32767
	NYA_PAD_KEY_LSTICK_Y_NEG, // -32768 - 32767
	NYA_PAD_KEY_LSTICK_Y_POS, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_X_NEG, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_X_POS, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_Y_NEG, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_Y_POS, // -32768 - 32767
	NYA_PAD_KEY_LSTICK_X, // -32768 - 32767
	NYA_PAD_KEY_LSTICK_Y, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_X, // -32768 - 32767
	NYA_PAD_KEY_RSTICK_Y, // -32768 - 32767
	NUM_NYA_PAD_KEYS
};
static const char* nyaPadKeyNames[NUM_NYA_PAD_KEYS] = {
		"None",
		"DPad Up",
		"DPad Down",
		"DPad Left",
		"DPad Right",
		"Start",
		"Select",
		"L3",
		"R3",
		"LB",
		"RB",
		"A",
		"B",
		"X",
		"Y",
		"LT",
		"RT",
		"Left Stick X-",
		"Left Stick X+",
		"Left Stick Y-",
		"Left Stick Y+",
		"Right Stick X-",
		"Right Stick X+",
		"Right Stick Y-",
		"Right Stick Y+",
};

bool IsKeyPressed(int key);
bool IsKeyJustPressed(int key);
bool IsPadKeyPressed(int key, int padId = -1);
bool IsPadKeyJustPressed(int key, int padId = -1);
int GetPadKeyState(int key, int padId = -1, bool padKeyAsButton = false);
int GetPadLastKeyState(int key, int padId = -1, bool padKeyAsButton = false);
std::string GetTimeFromSeconds(int value);
std::string GetTimeFromMilliseconds(int value);
bool StringHasEnding(std::string const &fullString, std::string const &ending);
std::string CapStringToWidth(const char* str, float size, float width);
bool IsWriteableInputCharacter(std::string& str);
void WriteStringToFile(std::ofstream& file, const char* string);
std::string ReadStringFromFile(std::ifstream& file);
DWORD XInputGetState_Dynamic(int dwUserIndex, XINPUT_STATE* pState);
DWORD XInputSetState_Dynamic(int dwUserIndex, XINPUT_VIBRATION* pState);
void CommonMain();