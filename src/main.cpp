#include <iostream>
#include <string.h>
#include <windows.h>
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/wglext.h"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const char* vertexShaderSource = "#version 330 core\n"
                             "layout (location = 0) in vec4 aPos;\n"
                             "void main() {\n"
                             "  gl_Position = aPos;\n"
                             "}";

const char* fragmentShaderSource = "#version 330 core\n"
                             "layout (location = 0) out vec4 FragColor;\n"
                             "void main() {\n"
                             "  FragColor = vec4(0.7, 0.3, 0.3, 1.0);\n"
                             "}";

static void CheckShaderErrors(unsigned int shader)
{
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success)
  { 
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* infoLog = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(shader, length, &length, infoLog);
    std::cout << "Shader Compilation Failed: " << infoLog << std::endl;
    glDeleteShader(shader);
  }
}

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

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
  RegisterWindowClass(hInstance);

  // legacy GL Context Initialization
  HWND fakeWindow = CreateWindow(
      "Basic",                          // Window class name
      "BasicOGL",                       // Window title
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

  // GL Context Initialization
  HWND Window = CreateWindow(
      "Basic",
      "OpenGL Window",
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

  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  CheckShaderErrors(vertexShader);

  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  CheckShaderErrors(fragmentShader);  

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glValidateProgram(shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "GLSL Program Linking failed: " << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  unsigned int vertexBuffer;
  float vertexPositions[6] = {
     0.0f, 0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f
    };

  unsigned int vertexArrayObject;
  glGenVertexArrays(1, &vertexArrayObject);
  glBindVertexArray(vertexArrayObject);

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexPositions, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  glUseProgram(shaderProgram);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
