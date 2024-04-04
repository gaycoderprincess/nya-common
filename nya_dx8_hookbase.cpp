#include <windows.h>
#include <cstdint>
#include <codecvt>
#include <locale>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "nya_dx8_hookbase.h"

IDirect3DDevice8* g_pd3dDevice = nullptr;

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

	ImGui_ImplDX8_Init(g_pd3dDevice);

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

// Simple helper function to load an image into a DX8 texture with common settings
PDIRECT3DTEXTURE8 LoadTexture(const char* filename)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
	if (!image_data)
		return nullptr;

	IDirect3DTexture8* texture = nullptr;
	g_pd3dDevice->CreateTexture(image_width, image_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture);

	if (!texture)
		return nullptr;

	D3DLOCKED_RECT rect;
	texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
	auto dest = (uint8_t*)rect.pBits;
	memcpy(dest, &image_data[0], image_width * image_height * 4);
	texture->UnlockRect(0);

	stbi_image_free(image_data);

	return texture;
}