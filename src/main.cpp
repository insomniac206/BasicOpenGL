#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/wglext.h"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;
  memset(&wcex, 0, sizeof(wcex));
  wcex.cbSize = sizeof(wcex);
  wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc = WindowProcedure;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.lpszClassName = L"Basic";

  return RegisterClassEx(&wcex);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
  RegisterWindowClass(hInstance);

  // legacy GL Context Initialization
  HWND fakeWindow = CreateWindow(
      L"Basic",                          // Window class name
      L"BasicOGL",                       // Window title
      WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Window style
      0, 0,                              // position (x, y)
      1, 1,                              // size (width, height)
      NULL, NULL,                        // Parent window, menu
      hInstance,
      NULL // params
  );

  HDC fakeDC = GetDC(fakeWindow);

  PIXELFORMATDESCRIPTOR fakePfd;
  memset(&fakePfd, 0, sizeof(fakePfd));
  fakePfd.nSize = sizeof(fakePfd);
  fakePfd.nVersion = 1;
  fakePfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  fakePfd.iPixelType = PFD_TYPE_RGBA;
  fakePfd.cColorBits = 32;
  fakePfd.cAlphaBits = 8;
  fakePfd.cDepthBits = 24;

  int fakePFDID = ChoosePixelFormat(fakeDC, &fakePfd);
  if (fakePFDID == 0)
  {
    std::cout << "ChoosePixelFormat() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (SetPixelFormat(fakeDC, fakePFDID, &fakePfd) == false)
  {
    std::cout << "SetPixelFormat() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  HGLRC fakeRC = wglCreateContext(fakeDC);

  if (fakeRC == 0)
  {
    std::cout << "wglCreateContext() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  if (wglMakeCurrent(fakeDC, fakeRC) == false)
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  // load necessary functions for creating new GL context
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatArb = nullptr;
  wglChoosePixelFormatArb = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
  if (wglChoosePixelFormatArb == nullptr)
  {
    std::cout << "wglGetProcAddress() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsArb = nullptr;
  wglCreateContextAttribsArb = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
  if (wglCreateContextAttribsArb == nullptr)
  {
    std::cout << "wglGetProcAddress() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  // GL 3.3 Context Initialization
  HWND Window = CreateWindow(
      L"Basic",
      L"OpenGL Window",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      700, 500,
      NULL, NULL,
      hInstance,
      NULL);

  HDC WndDC = GetDC(Window);

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

  bool status = wglChoosePixelFormatArb(WndDC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
  if (status == false || numFormats == 0)
  {
    std::cout << "wglChoosePixelFormatArb() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  PIXELFORMATDESCRIPTOR PFD;
  DescribePixelFormat(WndDC, pixelFormatID, sizeof(PFD), &PFD);
  SetPixelFormat(WndDC, pixelFormatID, &PFD);

  int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 3,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0};

  HGLRC RenderContext = wglCreateContextAttribsArb(WndDC, 0, contextAttribs);
  if (RenderContext == NULL)
  {
    std::cout << "wglCreateContextAttribsArb() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(fakeRC);
  ReleaseDC(fakeWindow, fakeDC);
  DestroyWindow(fakeWindow);

  if (!wglMakeCurrent(WndDC, RenderContext))
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  // function loader
  if (glewInit() != GLEW_OK)
  {
    std::cout << "glewInit() failed." << std::endl;
    std::cin.get();
    return 1;
  }

  ShowWindow(Window, nCmdShow);

  //TODO: render a triangle

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(WndDC);
  }

  return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE)
      PostQuitMessage(0);
    break;

  case WM_CLOSE:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return 0;
}
