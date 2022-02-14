#pragma once

#include <iostream>
#include <functional>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/wglext.h"

// sets the attributes of PIXELFORMATDESCRIPTOR struct, and sets pixel format, for legacy opengl context
void SetPFDLegacy(HDC DeviceContext);

// sets the attributes of PIXELFORMATDESCRIPTOR struct, and sets pixel format, for modern opengl context
void SetPFD(HDC DeviceContext, PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatArb);

HGLRC CreateGlContextLegacy(HDC DeviceContext);

HGLRC CreateGlContext(HDC DeviceContext, int GLVersionNumberMajor, int GLVersionNumberMinor, PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsArb);

WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, const char* ClassName, WNDPROC WindowProcedure);

HWND BGLCreateWindow(const char* ClassName, const char* WindowTitle, int WinPosX, int WinPosY, int wWidth, int wHeight, HINSTANCE hInstance);

template<typename glFuncPTR>
glFuncPTR load_function(const char* glFuncName)
{
  glFuncPTR GLFunc = nullptr;
  GLFunc = reinterpret_cast<glFuncPTR>(wglGetProcAddress(glFuncName));
  if (GLFunc == nullptr)
  {
    std::cout << "wglGetProcAddress() failed." << std::endl;
    std::cin.get();
    return nullptr;
  }

  return GLFunc;
}