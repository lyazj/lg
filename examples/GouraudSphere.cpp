#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLGouraudSphere.h"
#include "GLProgram.h"
#include "GLSimpleRenderableDecorator.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLUniformColorDecorator)

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLProgramPtr programs[2];
  GLint iProgram;
  GLRenderablePtr renderable;
  GLUniformColorDecoratorPtr sphere;
  GLuint nRightClicks = 0;

  void MouseDown(MouseButton, int x, int y) override;
  void KeyDown(unsigned char key, int x, int y) override;
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

  programs[0] = GLProgram::GetGouraudLightingProgram();
  programs[1] = GLProgram::GetDefaultLightingProgram();
  for(const auto &p : programs) p->SetDefaultHighlight({ 0.8f, 0.8f, 0.8f }, 50.0f, false);
  UseProgram(programs[iProgram = 0]);

  auto scene = make_shared<GLCompositeRenderable>();
  GLSimpleRenderablePtr sphere0 = make_shared<GLGouraudSphere>(0.2f, 3);
  sphere = make_shared<GLUniformColorDecorator>(sphere0, vec4(1.0f, 0.0f, 0.0f, 1.0f));
  scene->AddRenderable(sphere);
  scene->AddRenderable(make_shared<GLSimpleRenderableDecorator>(make_shared<GLAxes>(), GLProgram::GetDefaultProgram()));
  renderable = scene;

  renderable->SetVertexAttributes(false);
  renderable->Buffer(false);
  SetModel(rotate(mat4(1.0f), -45.0f * deg, vec3(0.0f, 1.0f, 0.0f)));
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::MouseDown(MouseButton b, int x, int y)
{
  GL3DApplication::MouseDown(b, x, y);

  if(b == MouseButton::RightButton) {
    switch(++nRightClicks) {
    case 1: sphere->SetColor(vec4(0.0f, 1.0f, 0.0f, 1.0f)); break;
    case 2: sphere->SetColor(vec4(0.0f, 0.0f, 1.0f, 1.0f)); break;
    case 3: sphere->SetColor(vec4(0.0f, 1.0f, 1.0f, 1.0f)); break;
    case 4: sphere->SetColor(vec4(1.0f, 0.0f, 1.0f, 1.0f)); break;
    case 5: sphere->SetColor(vec4(1.0f, 1.0f, 0.0f, 1.0f)); break;
    case 6: sphere->SetColor(vec4(1.0f, 1.0f, 1.0f, 1.0f)); break;
    case 7: sphere->SetColor(vec4(0.0f, 0.0f, 0.0f, 1.0f)); break;
    default: sphere->SetColor(vec4(RandVec3(), 1.0f)), nRightClicks = 8; break;  // no overflow
    }
  }
  PostRedisplay();
}

void GLExampleApplication::KeyDown(unsigned char key, int x, int y)
{
  GL3DApplication::KeyDown(key, x, y);

  if(toupper(key) == 'P') UseProgram(programs[iProgram = !iProgram]);
  PostRedisplay();
}
