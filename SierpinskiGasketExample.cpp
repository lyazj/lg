#include <glm/gtx/transform.hpp>

#include "GLApplication.h"
#include "GLProgram.h"
#include "GLSierpinskiGasketChaos.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLRenderablePtr geometry;
  mat4 trans;
  GLint ntrans;

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
  GLApplication::PreInit();

  SetTitle("Sierpinski Gasket Example");
  EnableDoubleBuffer();
}

void GLExampleApplication::Init()
{
  GLApplication::Init();
  glutIdleFunc([] { ((GLExampleApplication *)GLApplication::GetInstance())->Idle(); });

  UseProgram(GLProgram::GetDefaultProgram());

  geometry = make_shared<GLSierpinskiGasketChaos>(1e5);
  geometry->SetVertexAttributes();
  geometry->Buffer();

  mat4 m(1.0f);
  m = glm::scale(m, vec3(0.5f, 0.5f, 1.0f));
  m = glm::translate(m, vec3(0.0f, -1.0f, 0.0f));
  SetModel(m);

  trans = mat4(1.0f);
  trans = glm::translate(trans, vec3(sqrtf(3.0f) / 4.0f, 5.0f / 4.0f, 0.0f));
  trans = glm::scale(trans, vec3(0.5f, 0.5f, 1.0f));
  ntrans = 10;

  Display();
  SaveScreen("SierpinskiGasketExample.png");
}

void GLExampleApplication::Display()
{
  Clear();
  geometry->Draw(model);
  mat4 t = trans;
  for(int i = 0; i < ntrans; ++i) {
    geometry->Draw(model * t);
    t *= trans;
  }
  Flush();
}

void GLExampleApplication::Idle()
{
  static int ts;
  int t = (int)(GetElapsedTime() * 60ULL / 1000ULL);
  if(t == ts) return;
  ts = t;
  SetModel(glm::rotate(model, 2.0f * pi / 60.0f / 10.0f, vec3(0.0f, 0.0f, 1.0f)));
  glutPostRedisplay();
}
