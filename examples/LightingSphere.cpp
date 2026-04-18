#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLSimpleRenderableDecorator.h"
#include "GLSphere.h"

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
  GetProgram()->SetDefaultHighlight({ 0.2f, 0.2f, 0.2f }, 10.0f);

  auto scene = make_shared<GLCompositeRenderable>();
  GLSimpleRenderablePtr sphere = make_shared<GLSphere>(0.2f, 64, 32);
  sphere = make_shared<GLUniformColorDecorator>(sphere, vec4(1.0f, 0.0f, 0.0f, 1.0f));
  scene->AddRenderable(sphere);
  scene->AddRenderable(make_shared<GLSimpleRenderableDecorator>(make_shared<GLAxes>(), GLProgram::GetDefaultProgram()));
  renderable = scene;

  renderable->SetVertexAttributes(false);
  renderable->Buffer(false);
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
