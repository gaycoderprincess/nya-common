#ifndef NYA_DX11_BASE
#define NYA_DX11_BASE

#include "nya_commonbase.h"

extern const bool bVSync;
extern const bool bWindowed;
extern int nMouseWheelState;
extern std::string sProgramDir;
extern std::string sKeyboardInput;

bool InitAppBase();
void AppBaseLoop();
void ProgramLoop();
void ProgramOnExit();
#endif