#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLBezierSurface.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;
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
  GLApplication::PreInit();
  SetFrameRate(0);
  SetShowFrameRateInterval(0);
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  UseProgram(GLProgram::GetDefaultProgram());

  auto scene = make_shared<GLCompositeRenderable>();
  scene->AddRenderable(make_shared<GLAxes>());
  vec3 patch[16] = {
    vec3(-.15f, -.10f, -.15f), vec3(-.05f, -.08f, -.15f), vec3(+.05f, +.02f, -.15f), vec3(+.15f, +.04f, -.15f),  //
    vec3(-.15f, -.08f, -.05f), vec3(-.05f, -.06f, -.05f), vec3(+.05f, +.04f, -.05f), vec3(+.15f, +.06f, -.05f),  //
    vec3(-.15f, -.06f, +.05f), vec3(-.05f, -.04f, +.05f), vec3(+.05f, +.06f, +.05f), vec3(+.15f, +.08f, +.05f),  //
    vec3(-.15f, -.04f, +.15f), vec3(-.05f, -.02f, +.15f), vec3(+.05f, +.08f, +.15f), vec3(+.15f, +.10f, +.15f)   //
  };
  GLint division = 8;
  scene->AddRenderable(make_shared<GLBezierSurface>(patch, division));
  renderable = scene;

  renderable->SetVertexAttributes(false);
  renderable->Buffer(false);

  SetModel(glm::rotate(mat4(1.0f), 45.0f * deg, vec3(1.0f, 0.0f, 0.0f)));
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}
