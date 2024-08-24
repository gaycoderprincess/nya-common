#ifndef NYA_DX9_BASE
#define NYA_DX9_BASE

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