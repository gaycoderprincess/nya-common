#include <windows.h>
#include <cstdint>
#include <codecvt>
#include <locale>

#include "nya_dx11_appbase.h"

const auto sWindowName = TEXT(NYA_APP_WINDOWNAME);

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

const bool bVSync = true;
const bool bWindowed = true;
HWND ghWnd = nullptr;
int nResX = 640;
int nResY = 480;
int nMouseWheelState = 0;
std::string sProgramDir;
std::string sKeyboardInput;

void CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}
void CleanupRenderTarget() {
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}

void SetWindowResolution(int x, int y) {
	CleanupRenderTarget();
	g_pSwapChain->ResizeBuffers(0, x, y, DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTarget();
	nResX = x;
	nResY = y;
}

bool CreateD3DDevice() {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = ghWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bWindowed;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void AppBaseLoop() {
	nMouseWheelState = 0;
	sKeyboardInput = "";

	MSG msg;
	while (::PeekMessage(&msg, ghWnd, 0U, 0U, PM_REMOVE)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	ImVec4 clear_color = ImVec4(0, 0, 0, 1.00f);
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

	ProgramLoop();

	g_pSwapChain->Present(bVSync, 0);
}

bool IsWriteableInputCharacter(std::string& str) {
	if (str.find(VK_BACK) != std::string::npos) return false;
	if (str.find(VK_ESCAPE) != std::string::npos) return false;
	return true;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_MOUSEWHEEL:
			nMouseWheelState += GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			break;
		case WM_SIZE:
			if (g_pd3dDeviceContext && wParam != SIZE_MINIMIZED)
			{
				SetWindowResolution(LOWORD(lParam), HIWORD(lParam));
			}
			return 0;
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
			return 0;
		}
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			ProgramOnExit();
			exit(0);
		default:
			break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool InitAppBase() {
	WNDCLASSEX wndClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, sWindowName, nullptr };
	::RegisterClassEx(&wndClass);
	ghWnd = ::CreateWindow(wndClass.lpszClassName, sWindowName, WS_OVERLAPPEDWINDOW, 0, 0, nResX, nResY, nullptr, nullptr, wndClass.hInstance, nullptr);

	if (!CreateD3DDevice()) {
		::UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
		return false;
	}

	::ShowWindow(ghWnd, SW_SHOWDEFAULT);
	::UpdateWindow(ghWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	auto& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(NYA_FONTNAME, NYA_FONTSIZE);
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.DisplaySize.x = nResX;
	io.DisplaySize.y = nResY;

	char currentDir[MAX_PATH+1];
	GetCurrentDirectoryA(MAX_PATH, currentDir);
	sProgramDir = currentDir;

	FreeConsole();

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

// Simple helper function to load an image into a DX11 texture with common settings
ID3D11ShaderResourceView* LoadTexture(const char* filename) {
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
	if (!image_data)
		return nullptr;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = nullptr;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	ID3D11ShaderResourceView* ret;

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &ret);
	pTexture->Release();

	stbi_image_free(image_data);

	return ret;
}