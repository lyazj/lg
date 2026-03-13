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

  unsigned GetElapsedTime();
  void PostRedisplay() const;

  void SaveScreen(const fs::path &path, GLenum mode) const;

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

  enum class Mouse {
    Left,
    Middle,
    Right,
  };
  enum class SpecialKey {
    Up,
    Down,
    Left,
    Right,
  };

  virtual void PreInit();
  virtual void InitGL();
  virtual void Init();
  virtual void Display();
  virtual void Reshape(int w, int h);
  virtual void Idle();
  virtual void MouseDown(Mouse, int x, int y);
  virtual void MouseUp(Mouse, int x, int y);
  virtual void KeyDown(unsigned char key, int x, int y);
  virtual void KeyUp(unsigned char key, int x, int y);
  virtual void SpecialKeyDown(SpecialKey key, int x, int y);
  virtual void SpecialKeyUp(SpecialKey key, int x, int y);
  virtual void Loop();
};
