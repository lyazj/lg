#pragma once

#include <glm/mat4x4.hpp>

#include "Global.h"

class GLApplication {
public:
  GLApplication(int &argc, char *argv[]);
  virtual ~GLApplication();
  GLApplication(const GLApplication &) = delete;
  GLApplication &operator=(const GLApplication &) = delete;

  static GLApplication *GetInstance() { return gInstance; }

  virtual void Run();

  std::wstring GetProgramName() const { return programName; }
  std::wstring GetProgramShortName() const { return programShortName; }
  void SetDisplayMode(unsigned m);
  void SetWindowSize(int w, int h);
  GLint GetWindowWidth() const { return width; }
  GLint GetWindowHeight() const { return height; }
  void MaximizeWindow() const;
  GLfloat GetAspect() const;
  void SetTitle(const std::wstring &t);
  std::wstring GetDefaultTitle() const;
  const std::wstring &GetTitle() const { return title; }
  void EnableDoubleBuffer();
  void DisableDoubleBuffer();
  void Flush() const;

  void UseProgram(const GLProgramPtr &p);
  const GLProgramPtr &GetProgram() const { return program; }
  const mat4 &GetProjection() const { return projection; }
  const mat4 &GetView() const { return view; }
  const mat4 &GetModel() const { return model; }
  void SetProjection(const mat4 &p);
  void SetView(const mat4 &v);
  void SetModel(const mat4 &m);
  vec3 GetViewerPosition() const;

  void EnableBlend();
  void DisableBlend();
  void EnableDepthTest();
  void DisableDepthTest();
  void Clear() const;
  void PostRedisplay() const;

  // [NOTE] Caution: No synchronization. Avoid duplicate callback registrations.
  void SetFrameRate(GLint fr);
  GLint GetFrameRate() const { return frameRate; }
  GLfloat GetRealFrameRate() const { return (GLfloat)frameCount / ((GLfloat)frameTime / 1e9f); }
  void SetShowFrameRateInterval(GLint i);
  GLint GetShowFrameRateInterval() const { return showFrameRateInterval; }

  void SaveScreen(const fs::path &path, GLenum mode) const;
  void SaveScreen() const;

protected:
  // Single-window only: singleton routes GLUT callbacks.
  static GLApplication *gInstance;

  int &argc;
  char **argv;

  enum MouseButton : uint32_t {
    LeftButton = 1U << 0,
    MiddleButton = 1U << 1,
    RightButton = 1U << 2,
    WheelUp = 1U << 3,
    WheelDown = 1U << 4,
    WheelRight = 1U << 5,
    WheelLeft = 1U << 6,
    Backward = 1U << 7,
    Forward = 1U << 8,
  };

  enum SpecialKey : uint32_t {
    F1 = 1U << 0,
    F2 = 1U << 1,
    F3 = 1U << 2,
    F4 = 1U << 3,
    F5 = 1U << 4,
    F6 = 1U << 5,
    F7 = 1U << 6,
    F8 = 1U << 7,
    F9 = 1U << 8,
    F10 = 1U << 9,
    F11 = 1U << 10,
    F12 = 1U << 11,
    Up = 1U << 12,
    Down = 1U << 13,
    Left = 1U << 14,
    Right = 1U << 15,
    PageUp = 1U << 16,
    PageDown = 1U << 17,
    Home = 1U << 18,
    End = 1U << 19,
    Insert = 1U << 20,
    ShiftLeft = 1U << 21,
    ShiftRight = 1U << 22,
    CtrlLeft = 1U << 23,
    CtrlRight = 1U << 24,
    AltLeft = 1U << 25,
    AltRight = 1U << 26,
    SuperLeft = 1U << 27,
    SuperRight = 1U << 28,
  };

  virtual void PreInit();
  virtual void InitGL();
  virtual void Init();
  virtual void Display();
  virtual void Reshape(int w, int h);
  virtual void FrameTimer(unsigned mt);
  virtual void Frame(uint64_t t, uint64_t dt);
  virtual void MouseDown(MouseButton, int x, int y);
  virtual void MouseUp(MouseButton, int x, int y);
  virtual void MouseMove(int x, int y, int dx, int dy);
  virtual void KeyDown(unsigned char key, int x, int y);
  virtual void KeyUp(unsigned char key, int x, int y);
  virtual void SpecialKeyDown(SpecialKey key, int x, int y);
  virtual void SpecialKeyUp(SpecialKey key, int x, int y);
  virtual void Loop();

  uint32_t GetPressedSpecialKeys() const { return pressedSpecialKeys; }
  uint32_t GetPressedMouseButtons() const { return pressedMouseButtons; }
  void ShowFrameRate() const;

private:
  std::wstring programName;
  std::wstring programShortName;
  unsigned displayMode;
  int width, height;
  std::wstring title;
  int windowId;
  mat4 projection, view, model;
  GLProgramPtr program;
  GLbitfield clearMask;
  GLint frameRate;
  GLint showFrameRateInterval;
  uint64_t frameCount;
  uint64_t frameTime;
  uint32_t pressedSpecialKeys;
  static_assert(sizeof(pressedSpecialKeys) >= sizeof(SpecialKey));
  int mouseX, mouseY;
  uint32_t pressedMouseButtons;
  static_assert(sizeof(pressedMouseButtons) >= sizeof(MouseButton));
};

class GLProgramGuard {
public:
  GLProgramGuard(const GLProgramPtr &p);
  GLProgramGuard(const GLProgramGuard &) = delete;
  GLProgramGuard &operator=(const GLProgramGuard &) = delete;
  ~GLProgramGuard();

private:
  GLProgramPtr program;
};
