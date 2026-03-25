#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLSimpleRenderableDecorator.h"
#include "GLSphere.h"
#include "GLTransformedRenderable.h"

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;

  void Frame(uint64_t t, uint64_t dt) override;
};

int main(int argc, char *argv[])
{
  static GLApplicationPtr application;
  application = make_shared<GLExampleApplication>(argc, argv);
  application->Run();
  return 0;
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();

  UseProgram(GLProgram::GetDefaultLightingProgram());

  auto scene = make_shared<GLCompositeRenderable>();
  GLSimpleRenderablePtr sphere0 = make_shared<GLSphere>(0.2f, 64, 32);
  sphere0 = make_shared<GLUniformColorDecorator>(sphere0, vec4(1.0f, 0.0f, 0.0f, 1.0f));
  auto sphere1 = make_shared<GLTransformedRenderable>(sphere0, rotate(mat4(1.0f), 20.0f * deg, vec3(0.0f, 1.0f, 0.0f)));
  scene->AddRenderable(sphere1);
  scene->AddRenderable(make_shared<GLSimpleRenderableDecorator>(make_shared<GLAxes>(), GLProgram::GetDefaultProgram()));
  renderable = scene;

  renderable->SetVertexAttributes();
  renderable->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt)
{
  SetModel(rotate(GetModel(), 2.0f * pi * (GLfloat)dt / 1e10f, vec3(0.0f, 1.0f, 0.0f)));
}
