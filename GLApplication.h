#pragma once

#include <filesystem>
#include <glm/mat4x4.hpp>
#include <string>

#include "Global.h"

class GLApplication {
public:
  GLApplication(int &argc, char *argv[]);
  virtual ~GLApplication();
  GLApplication(const GLApplication &) = delete;
  GLApplication &operator=(const GLApplication &) = delete;

  static GLApplication *GetInstance() { return tInstance; }

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
  void SetProjection(const glm::mat4 &p);
  void SetView(const glm::mat4 &v);
  void SetModel(const glm::mat4 &m);

  void EnableBlend();
  void DisableBlend();
  void EnableDepthTest();
  void DisableDepthTest();
  void Clear() const;

  void SaveScreen(const fs::path &path) const;

protected:
  // Single-window only: singleton routes GLUT callbacks.
  inline static thread_local GLApplication *tInstance;

  int &argc;
  char **argv;
  unsigned displayMode;
  int width, height;
  std::string title;
  int windowId;
  glm::mat4 projection, view, model;
  void (*flush)();
  GLProgramPtr program;
  GLbitfield clearMask;

  virtual void PreInit();
  virtual void InitGL();
  virtual void Init();
  virtual void Display() { }
  virtual void Reshape(int w, int h);
  virtual void Loop() { glutMainLoop(); }
};
