#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLCuboid.h"
#include "GLProgram.h"

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

class CuboidFaceColorDecorator : public GLBufferedColorDecorator {
public:
  CuboidFaceColorDecorator(const GLCuboid &cuboid, GLint i);

protected:
  void SetColors() const override { }
};

CuboidFaceColorDecorator::CuboidFaceColorDecorator(const GLCuboid &c, GLint i) : GLBufferedColorDecorator(c.GetFace(i))
{
  colors.reserve(4);
  for(GLint j = 0; j < 4; ++j) {
    vec3 v;
    c.GetVertex(i, j, v);
    colors.emplace_back(v.x >= 0.0f, v.y >= 0.0f, v.z >= 0.0f, 1.0f);
  }
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  SetView({ 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, -1.0f });
  UseProgram(GLProgram::GetDefaultLightingProgram());

  auto scene = make_shared<GLCompositeRenderable>();
  auto cubic = make_shared<GLCuboid>(0.5f, 0.5f, 0.5f);
  for(GLint i = 0; i < 6; ++i) cubic->SetFace(i, make_shared<CuboidFaceColorDecorator>(*cubic, i));
  scene->AddRenderable(cubic);
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
