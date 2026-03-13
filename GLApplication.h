#pragma once

#include <glm/mat4x4.hpp>
#include <string>

#include "Global.h"

class GLApplication {
public:
  GLApplication(int &argc, char *argv[]);
  virtual ~GLApplication();
  GLApplication(const GLApplication &) = delete;
  GLApplication &operator=(const GLApplication &) = delete;

  static GLApplication *GetInstance() { return gInstance; }

  virtual void Run();

  std::string GetProgramName() const { return programName; }
  std::string GetProgramShortName() const { return programShortName; }
  void SetDisplayMode(unsigned m);
  void SetWindowSize(int w, int h);
  void SetTitle(const std::string &t);
  GLfloat GetAspect() const { return (GLfloat)width / (GLfloat)height; }
  void EnableDoubleBuffer();
  void DisableDoubleBuffer();
  void Flush() const { flush(); }

  void UseProgram(const GLProgramPtr &p);
  const GLProgramPtr &GetProgram() const { return program; }
  void SetProjection(const mat4 &p);
  void SetView(const mat4 &v);
  void SetModel(const mat4 &m);

  void EnableBlend();
  void DisableBlend();
  void EnableDepthTest();
  void DisableDepthTest();
  void Clear() const;
  void PostRedisplay() const;

  void SetFrameRate(GLint fr) { frameRate = fr; }
  GLint GetFrameRate() const { return frameRate; }
  GLfloat GetRealFrameRate() const { return (GLfloat)frameCount / ((GLfloat)frameTime / 1e9f); }

  void SaveScreen(const fs::path &path, GLenum mode) const;
  void SaveScreen() const;

protected:
  // Single-window only: singleton routes GLUT callbacks.
  static GLApplication *gInstance;

  int &argc;
  char **argv;
  std::string programName;
  std::string programShortName;
  unsigned displayMode;
  int width, height;
  std::string title;
  int windowId;
  mat4 projection, view, model;
  void (*flush)();
  GLProgramPtr program;
  GLbitfield clearMask;
  GLint frameRate;
  GLint showFrameRateInterval;
  uint64_t frameCount;
  uint64_t frameTime;

  // clang-format off
  enum class Mouse {
    LeftButton, MiddleButton, RightButton,
    WheelUp, WheelDown, WheelRight, WheelLeft,
    Backward, Forward,
  };
  // clang-format on

  // clang-format off
  enum class SpecialKey {
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Up, Down, Left, Right,
    PageUp, PageDown, Home, End, Insert,
    ShiftLeft, ShiftRight, CtrlLeft, CtrlRight, AltLeft, AltRight, SuperLeft, SuperRight,
  };
  // clang-format on

  virtual void PreInit();
  virtual void InitGL();
  virtual void Init();
  virtual void Display();
  virtual void Reshape(int w, int h);
  virtual void FrameTimer(unsigned mt);
  virtual void Frame(uint64_t t, uint64_t dt);
  virtual void MouseDown(Mouse, int x, int y);
  virtual void MouseUp(Mouse, int x, int y);
  virtual void KeyDown(unsigned char key, int x, int y);
  virtual void KeyUp(unsigned char key, int x, int y);
  virtual void SpecialKeyDown(SpecialKey key, int x, int y);
  virtual void SpecialKeyUp(SpecialKey key, int x, int y);
  virtual void Loop();

  void GetKeyModifiers(bool &shift, bool &ctrl, bool &alt, bool &super) const;
  void ShowFrameRate() const;
};
