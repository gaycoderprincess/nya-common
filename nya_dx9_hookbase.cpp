#include <windows.h>
#include <cstdint>
#include <codecvt>
#include <locale>
#ifndef NYA_COMMON_NO_D3DX9
#include <d3dx9.h>
#endif

#include "nya_dx9_hookbase.h"

IDirect3DDevice9* g_pd3dDevice = nullptr;

HWND ghWnd = nullptr;
int nResX = 640;
int nResY = 480;
int nMouseWheelState = 0;
std::string sProgramDir;
std::string sKeyboardInput;

void HookBaseLoop() {
	HookLoop();

	nMouseWheelState = 0;
	sKeyboardInput = "";
}

void WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_MOUSEWHEEL:
			nMouseWheelState += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			break;
		case WM_CHAR: {
			std::string inputStr;
			if (IsWindowUnicode(hWnd)) {
				std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
				if (wParam > 0 && wParam < 0x10000) {
					std::wstring str;
					str += (wchar_t) wParam;
					inputStr = utf8_conv.to_bytes(str);
				}
			} else {
				inputStr += (char) wParam;
			}
			if (inputStr[0] && IsWriteableInputCharacter(inputStr)) sKeyboardInput += inputStr;
		}
		default:
			break;
	}
}

bool InitHookBase() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplDX9_Init(g_pd3dDevice);

	auto& io = ImGui::GetIO();
#ifndef NYA_NO_FONT
	io.Fonts->AddFontFromFileTTF(NYA_FONTNAME, NYA_FONTSIZE);
#endif
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.DisplaySize.x = nResX;
	io.DisplaySize.y = nResY;

	char currentDir[MAX_PATH+1];
	GetCurrentDirectoryA(MAX_PATH, currentDir);
	sProgramDir = currentDir;

	return true;
}

double GetMousePosX() {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(ghWnd, &point);
	return point.x / (double)nResX;
}
double GetMousePosY() {
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(ghWnd, &point);
	return point.y / (double)nResY;
}

double GetAspectRatio() {
	return GetAspectRatioDefault();
}

bool IsWindowInFocus() {
	return GetForegroundWindow() == ghWnd;
}

int GetMouseWheelState() {
	return nMouseWheelState;
}

#ifndef NYA_COMMON_NO_D3DX9
PDIRECT3DTEXTURE9 LoadTexture(const char* fileName) {
	PDIRECT3DTEXTURE9 texture;
	auto hr = D3DXCreateTextureFromFileA(g_pd3dDevice, fileName, &texture);
	if (hr != S_OK) {
		return nullptr;
	}
	return texture;
}
#endif