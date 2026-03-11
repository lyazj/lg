#include <glm/gtx/transform.hpp>

#include "GL3DApplication.h"
#include "GLProgram.h"
#include "GLTriangle.h"

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLRenderablePtr geometry;
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

  SetTitle("GLTriangle Example");
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  UseProgram(GLProgram::GetDefaultProgram());

  GLfloat r = 0.2f;
  vec3 v1(0.0f, r, 0.0f);
  vec3 v2(-sqrtf(3.0f) * 0.5f * r, -0.5f * r, 0.0f);
  vec3 v3(+sqrtf(3.0f) * 0.5f * r, -0.5f * r, 0.0f);
  geometry = make_shared<GLTriangle>(v1, v2, v3);
  geometry->SetVertexAttributes();
  geometry->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  mat4 m;

  GLProgram::SetVertexAttribute("a_color", vec4(1.0, 0.0, 0.0, 1.0));
  m = mat4(1.0f);
  geometry->Draw(model * m);

  GLProgram::SetVertexAttribute("a_color", vec4(0.0, 1.0, 0.0, 1.0));
  m = mat4(1.0f);
  m = glm::rotate(m, 20.0f * deg, vec3(0.0f, 0.0f, 1.0f));
  geometry->Draw(model * m);

  GLProgram::SetVertexAttribute("a_color", vec4(0.0, 0.0, 1.0, 1.0));
  m = mat4(1.0f);
  m = glm::rotate(m, 40.0f * deg, vec3(0.0f, 0.0f, 1.0f));
  m = glm::translate(m, vec3(0.0f, 0.0f, 0.2f));
  geometry->Draw(model * m);

  Flush();
}
