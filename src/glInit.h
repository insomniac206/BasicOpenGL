#include <iostream>
#include <functional>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/wglext.h"

// sets the attributes of PIXELFORMATDESCRIPTOR struct, and sets pixel format, for legacy opengl context
void SetPFDLegacy(HDC DeviceContext);

// sets the attributes of PIXELFORMATDESCRIPTOR struct, and sets pixel format, for modern opengl context
void SetPFD(HDC DeviceContext);

HGLRC CreateGlContextLegacy(HDC DeviceContext);

HGLRC CreateGlContext(HDC DeviceContext, int GLVersionNumberMajor, int GLVersionNumberMinor);

WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, const char* ClassName, WNDPROC WindowProcedure);

HWND BGLCreateWindow(const char* ClassName, const char* WindowTitle, int WinPosX, int WinPosY, int wWidth, int wHeight, HINSTANCE hInstance);

HGLRC CreateGLContext(HDC DeviceContext);

template<typename glFuncPTR>
glFuncPTR load_function(const char* glFuncName);