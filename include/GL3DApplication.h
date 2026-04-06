#pragma once

#include <glm/vec3.hpp>

#include "GLApplication.h"

// Undefine Windows macros...
#undef near
#undef far

class GL3DApplication : public GLApplication {
public:
  GL3DApplication(int &argc, char *argv[]);
  ~GL3DApplication() override;

  GLfloat GetFov() const { return fov; }
  GLfloat GetNear() const { return near; }
  GLfloat GetFar() const { return far; }
  void SetProjection(GLfloat fov, GLfloat near, GLfloat far);

  const vec3 &GetEye() const { return eye; }
  const vec3 &GetCenter() const { return center; }
  const vec3 &GetUp() const { return up; }
  void SetView(const vec3 &eye, const vec3 &center, const vec3 &up);

protected:
  void Init() override;
  void Reshape(int w, int h) override;
  void MouseMove(int x, int y, int dx, int dy) override;

private:
  using GLApplication::SetProjection;
  using GLApplication::SetView;

  GLfloat fov, near, far;
  vec3 eye, center, up;
};
