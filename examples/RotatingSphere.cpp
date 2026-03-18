#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLSphere.h"
#include "GLTransformedRenderable.h"

using namespace std;

class GLExampleSphere : public GLBufferedColorDecorator {
public:
  GLExampleSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLExampleSphere() override = default;

protected:
  void SetColors() const override { }
};

GLExampleSphere::GLExampleSphere(GLfloat r, GLint sl, GLint st)
    : GLBufferedColorDecorator(make_shared<GLSphere>(r, sl, st))
{
  GLint n = renderable->GetNVertex();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) {
    vec3 vertex;
    renderable->GetVertex(i, vertex);
    GLfloat lambda = vertex.z / r / 2.0f + 0.5f;
    colors.emplace_back(lambda, lambda, lambda, 1.0f);
  }
}

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

  UseProgram(GLProgram::GetDefaultProgram());

  auto scene = make_shared<GLCompositeRenderable>();
  GLRenderablePtr sphere = make_shared<GLExampleSphere>(0.2f, 64, 32);
  sphere = make_shared<GLTransformedRenderable>(sphere, rotate(mat4(1.0f), 20.0f * deg, vec3(0.0f, 1.0f, 0.0f)));
  scene->AddGeometry(sphere);
  scene->AddGeometry(make_shared<GLAxes>());
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
