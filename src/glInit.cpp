#include "glInit.h"


WNDCLASSEX CreateWindowClass(HINSTANCE hInstance, const wchar_t ClassName, function<LRESULT(HWND, UINT, WPARAM, LPARAM)> WindowProcedure)
{
  WNDCLASSEX WindowClass;
  memset(&WindowClass, 0, sizeof(WindowClass));
  WindowClass.cbSize = sizeof(WindowClass);
  WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass.lpfnWndProc = WindowProcedure;
  WindowClass.hInstance = hInstance;
  WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  WindowClass.lpszClassName = ClassName;

  return WindowClass;
}


HWND CreateWindow(const wchar_t ClassName, const wchar_t WindowTitle, int WinPosX, int WinPosY, int wWidth, int wHeight, HINSTNACE hInstance)
{
  HWND Window = CreateWindow(
    ClassName,
    WindowTitle,
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    WinPosX, WinPosY,
    wWidth, wHeight,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  return Window;
}


HGLRC CreateGLContext(HDC DeviceContext)
{
  HGLRC RenderContext = wglCreateContext(DeviceContext);

  if (RenderContext == 0)
  {
    std::cout << "wglCreateContext() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (wglMakeCurrent(DeviceContext, RenderContext) == false)
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
    std::cin.get();
    return 1;
  }
}


template<typename glFuncPTR>
glFuncPTR load_function(const wchar_t glFuncName)
{
  glFuncPTR GLFunc = nullptr;
  GLFunc = reinterpret_cast<glFuncPTR>(wglGetProcAddress(glFuncName));
  if (wglChoosePixelFormatArb == nullptr)
  {
    std::cout << "wglGetProcAddress() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  return GLFunc;
}


void SetPFDLegacy(HDC DeviceContext)
{
  PIXELFORMATDESCRIPTOR Pfd;
  memset(&Pfd, 0, sizeof(Pfd));
  Pfd.nSize = sizeof(Pfd);
  Pfd.nVersion = 1;
  Pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  Pfd.iPixelType = PFD_TYPE_RGBA;
  Pfd.cColorBits = 32;
  Pfd.cAlphaBits = 8;
  Pfd.cDepthBits = 24;

  int PFDID = ChoosePixelFormat(DeviceContext, &Pfd);
  if (PFDID == 0)
  {
    std::cout << "ChoosePixelFormat() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (SetPixelFormat(DeviceContext, PFDID, &Pfd) == false)
  {
    std::cout << "SetPixelFormat() failed." << std::endl;
    std::cin.get();
    return 1;
  }
}


void SetPFD(HDC DeviceContext)
{
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatArb = load_function<PFNWGLCHOOSEPIXELFORMATARBPROC>("wglChoosePixelFormatARB");

  const int pixelAttribs[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_ALPHA_BITS_ARB, 8,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,
      WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
      WGL_SAMPLES_ARB, 4,
      0};

  int pixelFormatID;
  UINT numFormats;

  bool status = wglChoosePixelFormatArb(DeviceContext, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
  if (status == false || numFormats == 0)
  {
    std::cout << "wglChoosePixelFormatArb() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  PIXELFORMATDESCRIPTOR PFD;
  DescribePixelFormat(DeviceContext, pixelFormatID, sizeof(PFD), &PFD);
  SetPixelFormat(DeviceContext, pixelFormatID, &PFD);
}


void CreateGlContextLegacy(HDC DeviceContext)
{
  HGLRC RenderContext = wglCreateContext(DeviceContext);

  if (RenderContext == 0)
  {
    std::cout << "wglCreateContext() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (wglMakeCurrent(DeviceContext, RenderContext) == false)
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
    std::cin.get();
    return 1;
  }
}


void CreateGlContext(HDC DeviceContext, int GLVersionNumberMajor, int GLVersionNumberMinor)
{
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsArb = load_function<PFNWGLCREATECONTEXTATTRIBSARBPROC>("wglCreateContextAttribsARB");

  int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, GLVersionNumberMajor,
      WGL_CONTEXT_MINOR_VERSION_ARB, GLVersionNumberMinor,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0};

  HGLRC RenderContext = wglCreateContextAttribsArb(DeviceContext, 0, contextAttribs);
  if (RenderContext == NULL)
  {
    std::cout << "wglCreateContextAttribsArb() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (!wglMakeCurrent(DeviceContext, RenderContext))
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
    std::cin.get();
    return 1;
  }
}
