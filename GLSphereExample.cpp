#include <glm/gtx/transform.hpp>
#include <vector>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLSphere.h"
#include "GLTransformedRenderable.h"
#include "Utils.h"

using namespace std;

class GLExampleSphere : public GLBufferedColorDecorator {
public:
  GLExampleSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLExampleSphere() override = default;

protected:
  void SetColor() const override { }
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

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;
  void Idle();
};

int main(int argc, char *argv[])
{
  static GLApplicationPtr application;
  application = make_shared<GLExampleApplication>(argc, argv);
  application->Run();
  return 0;
}

void GLExampleApplication::PreInit()
{
  GL3DApplication::PreInit();

  SetTitle("GLSphere Example");
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  glutIdleFunc([] { ((GLExampleApplication *)GLApplication::GetInstance())->Idle(); });

  UseProgram(GLProgram::GetDefaultProgram());

  auto box = make_shared<GLCompositeRenderable>();
  GLRenderablePtr sphere = make_shared<GLExampleSphere>(0.2, 64, 32);
  sphere = make_shared<GLTransformedRenderable>(sphere, glm::rotate(20.0f * deg, vec3(0.0f, 1.0f, 0.0f)));
  box->AddGeometry(sphere);
  box->AddGeometry(make_shared<GLAxes>());
  renderable = box;

  renderable->SetVertexAttributes();
  renderable->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(model);
  Flush();
}

void GLExampleApplication::Idle()
{
  static int ts;
  int t = (int)(GetElapsedTime() * 60ULL / 1000ULL);
  if(t == ts) return;
  ts = t;
  SetModel(glm::rotate(model, 2.0f * pi / 60.0f / 10.0f, vec3(0.0f, 1.0f, 0.0f)));
  glutPostRedisplay();
}
