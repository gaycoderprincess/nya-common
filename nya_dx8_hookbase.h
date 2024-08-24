#ifndef NYA_DX8_BASE
#define NYA_DX8_BASE

#include "nya_commonbase.h"

extern int nMouseWheelState;
extern std::string sProgramDir;
extern std::string sKeyboardInput;

bool InitHookBase();
void HookBaseLoop();
void WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void HookLoop();
void HookOnExit();
#endif