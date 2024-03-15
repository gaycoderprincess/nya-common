// predefined stuff for program-specific but important values
#include "imgui.h"

#ifdef NYA_BACKEND_DX11
#include <d3d11.h>
#include "imgui_impl_dx11.h"

#include "stb_image.h"

#define TEXTURE_TYPE ID3D11ShaderResourceView

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
#endif

extern HWND ghWnd;
extern int nResX;
extern int nResY;
int GetMouseWheelState();
double GetMousePosX();
double GetMousePosY();
bool IsWindowInFocus();
double GetAspectRatio(); // while the default func should work in 90% of cases, other ratios might be manually selected
TEXTURE_TYPE* LoadTexture(const char* filename);