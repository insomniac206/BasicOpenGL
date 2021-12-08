#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>
#include "glad/glad.h"

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
  wcex.lpszClassName = "Basic";

  return RegisterClassEx(&wcex);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
  RegisterWindowClass(hInstance);

  HWND fakeWindow = CreateWindow(
      "Basic",                           // Window class name
      "BasicOGL",                        // Window title
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
    return 1;
  }

  if (SetPixelFormat(fakeDC, fakePFDID, &fakePfd) == false)
  {
    std::cout << "SetPixelFormat() failed." << std::endl;
    return 1;
  }

  HGLRC fakeRC = wglCreateContext(fakeDC);

  if (fakeRC == 0)
  {
    std::cout << "wglCreateContext() failed." << std::endl;
    return 1;
  }

  if (wglMakeCurrent(fakeDC, fakeRC) == false)
  {
    std::cout << "wglMakeCurrent() failed." << std::endl;
  }

  if (!gladLoadGL())
  {
    std::cout << "Failed to initialize opengl context." << std::endl;
    return 1;
  }

  HWND Window = CreateWindow(
      "Basic",
      "OpenGL Window",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT,
      NULL, NULL,
      hInstance,
      NULL);

  HDC WndDC = GetDC(Window);

  // TODO: Use modern openGL functions to create modern context, and then render a triangle.

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

