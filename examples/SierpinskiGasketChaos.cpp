#include <glm/gtc/matrix_transform.hpp>

#include "GLApplication.h"
#include "GLProgram.h"
#include "GLSierpinskiGasketChaos.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;
  mat4 trans;
  GLint ntrans;

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
  GLApplication::Init();
  UseProgram(GLProgram::GetDefaultProgram());

  renderable = make_shared<GLSierpinskiGasketChaos>(1e5);
  renderable->SetVertexAttributes();
  renderable->Buffer();

  mat4 m(1.0f);
  m = glm::scale(m, vec3(0.5f, 0.5f, 1.0f));
  m = glm::translate(m, vec3(0.0f, -1.0f, 0.0f));
  SetModel(m);

  trans = mat4(1.0f);
  trans = glm::translate(trans, vec3(sqrtf(3.0f) / 4.0f, 5.0f / 4.0f, 0.0f));
  trans = glm::scale(trans, vec3(0.5f, 0.5f, 1.0f));
  ntrans = 10;

  Display();
  SaveScreen("SierpinskiGasketChaos.png", GL_FRONT);
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  mat4 t = trans;
  for(GLint i = 0; i < ntrans; ++i) {
    renderable->Draw(GetModel() * t);
    t *= trans;
  }
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt)
{
  SetModel(glm::rotate(GetModel(), 2.0f * pi * (GLfloat)dt / 1e10f, vec3(0.0f, 0.0f, 1.0f)));
}
