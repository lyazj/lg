#include "GL3DApplication.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace std;

GL3DApplication::GL3DApplication(int &ac, char *av[])
    : GLApplication(ac, av),
      fov(45.0f * deg),
      near(0.1f),
      far(10.0f),
      eye(0.0f, 0.0f, 1.0f),
      center(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f)
{
  // empty
}

GL3DApplication::~GL3DApplication()
{
  // empty
}

void GL3DApplication::SetProjection(GLfloat f, GLfloat n, GLfloat fr)
{
  SetProjection(glm::perspective(f, GetAspect(), n, fr));
  fov = f;
  near = n;
  far = fr;
}

void GL3DApplication::SetView(const vec3 &e, const vec3 &c, const vec3 &u)
{
  SetView(lookAt(e, c, u));
  eye = e;
  center = c;
  up = u;
}

void GL3DApplication::Init()
{
  GLApplication::Init();

  EnableDepthTest();

  SetView(lookAt(eye, center, up));
  SetProjection(glm::perspective(fov, GetAspect(), near, far));
  SetModel(mat4(1.0f));
}

void GL3DApplication::Reshape(int w, int h)
{
  GLApplication::Reshape(w, h);

  SetProjection(glm::perspective(fov, GetAspect(), near, far));
}

void GL3DApplication::MouseMove(int x, int y, int dx, int dy)
{
  GLApplication::MouseMove(x, y, dx, dy);

  bool ctrl = GetPressedSpecialKeys() & (SpecialKey::CtrlLeft | SpecialKey::CtrlRight);
  bool leftButton = GetPressedMouseButtons() & MouseButton::LeftButton;

  if(ctrl && leftButton) {
    // Negative sign: rotate the camera opposite to mouse motion (equivalent to rotating the scene).
    GLfloat theta_y = -(GLfloat)dx / (GLfloat)GetWindowWidth() * 360.0f * deg;
    GLfloat theta_x = -(GLfloat)dy / (GLfloat)GetWindowHeight() * 360.0f * deg;

    vec3 ez = normalize(eye - center);
    vec3 ey = normalize(up);
    vec3 ex = normalize(cross(ey, ez));
    ey = cross(ez, ex);  // Conventionally y is not necessarily up.

    mat4 transform = mat4(1.0f);
    transform = rotate(transform, theta_x, ex);
    transform = rotate(transform, theta_y, ey);
    eye = mat3(transform) * (eye - center) + center;
    up = mat3(transform) * up;
    SetView(lookAt(eye, center, up));
  }

  PostRedisplay();
}
