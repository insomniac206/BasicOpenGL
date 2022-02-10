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

void CreateGlContextLegacy(HDC DeviceContext);

void CreateGlContext(HDC DeviceContext, int GLVersionNumberMajor, int GLVersionNumberMinor);

WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, const wchar_t ClassName, function<LRESULT CALLBACK(HWND, UINT, WPARAM, LPARAM)> WindowProcedure);

HWND CreateWindow(const wchar_t ClassName, const wchar_t WindowTitle, int WinPosX, int WinPosY, int wWidth, int wHeight, HINSTANCE hInstance);

HGLRC CreateGLContext(HDC DeviceContext);

template<typename glFuncPTR>
glFuncPTR load_function(const wchar_t glFuncName);
