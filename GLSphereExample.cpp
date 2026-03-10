#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLBuffer.h"
#include "GLCompositeGeometry.h"
#include "GLProgram.h"
#include "GLSphere.h"
#include "GLTransformedGeometry.h"
#include "Utils.h"

using namespace std;

class GLExampleSphere : public GLSphere {
public:
  GLExampleSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLExampleSphere() override = default;

  void SetVertexAttributes() const override;

protected:
  GLBuffer colorBuffer;
  std::vector<glm::vec4> colors;

  void IssueBuffer() const override;
};

GLExampleSphere::GLExampleSphere(GLfloat r, GLint sl, GLint st) : GLSphere(r, sl, st)
{
  colors.reserve(vertices.size());
  for(const glm::vec3 &vertex : vertices) {
    GLfloat lambda = vertex.z / radius / 2.0f + 0.5f;
    colors.emplace_back(lambda, lambda, lambda, 1.0f);
  }
}

void GLExampleSphere::SetVertexAttributes() const
{
  GLSphere::SetVertexAttributes();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
}

void GLExampleSphere::IssueBuffer() const
{
  GLSphere::IssueBuffer();
  colorBuffer.Buffer(colors);
}

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLGeometryPtr geometry;
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

  auto box = make_shared<GLCompositeGeometry>();
  GLGeometryPtr sphere = make_shared<GLExampleSphere>(0.2, 64, 32);
  sphere = make_shared<GLTransformedGeometry>(sphere, glm::rotate(20.0f * deg, glm::vec3(0.0f, 1.0f, 0.0f)));
  box->AddGeometry(sphere);
  box->AddGeometry(make_shared<GLAxes>());
  geometry = box;

  geometry->SetVertexAttributes();
  geometry->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  geometry->Draw(model);
  Flush();
}

void GLExampleApplication::Idle()
{
  static int ts;
  int t = (int)(GetElapsedTime() * 60ULL / 1000ULL);
  if(t == ts) return;
  ts = t;
  SetModel(glm::rotate(model, 2.0f * pi / 60.0f / 10.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
  glutPostRedisplay();
}
