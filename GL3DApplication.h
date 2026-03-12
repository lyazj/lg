#pragma once

#include "GLApplication.h"

// Undefine Windows macros...
#undef near
#undef far

class GL3DApplication : public GLApplication {
public:
  GL3DApplication(int &argc, char *argv[]);
  ~GL3DApplication() override;

  void SetProjection(GLfloat fov, GLfloat near, GLfloat far);

protected:
  using GLApplication::SetProjection;

  void Init() override;
  void Reshape(int w, int h) override;

  GLfloat fov, near, far;
};
