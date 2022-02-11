#include "glInit.h"

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParamm);

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


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
  CreateWindowClass(hInstance, "Basic", &WindowProcedure);

  HWND fakeWindow = BGLCreateWindow("Basic", "BasicOGL", 0, 0, 1, 1, hInstance);

  HDC fakeDC = GetDC(fakeWindow);

  SetPFDLegacy(fakeDC);
  HGLRC fakeRenderContext = CreateGlContextLegacy(fakeDC);

  HWND Window = BGLCreateWindow("Basic", "OpenGL Window", CW_USEDEFAULT, CW_USEDEFAULT, 700, 500, hInstance);

  HDC DeviceContext = GetDC(Window);

  SetPFD(DeviceContext);

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(fakeRenderContext);
  ReleaseDC(fakeWindow, fakeDC);
  DestroyWindow(fakeWindow);

  HGLRC RenderContext = CreateGlContext(DeviceContext, 3, 3);

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
    SwapBuffers(DeviceContext);
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