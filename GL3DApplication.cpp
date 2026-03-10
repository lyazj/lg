#include "GL3DApplication.h"

#include <glm/gtx/transform.hpp>

using namespace std;

GL3DApplication::GL3DApplication(int &ac, char *av[]) : GLApplication(ac, av), fov(45.0f * deg), near(0.1f), far(10.0f)
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

void GL3DApplication::Init()
{
  GLApplication::Init();

  EnableDepthTest();

  glm::vec3 camera(0.0f, 0.0f, 1.0f);
  glm::vec3 target(0.0f, 0.0f, 0.0f);
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  SetView(glm::lookAt(camera, target, up));
  SetProjection(glm::perspective(fov, GetAspect(), near, far));
  SetModel(glm::mat4(1.0f));
}

void GL3DApplication::Reshape(int w, int h)
{
  GLApplication::Reshape(w, h);

  SetProjection(glm::perspective(fov, GetAspect(), near, far));
}
