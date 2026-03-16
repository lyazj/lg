#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLImage.h"
#include "GLProgram.h"
#include "GLSphere.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "GLTransformedRenderable.h"

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
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

void GLExampleApplication::PreInit()
{
  GL3DApplication::PreInit();

  SetTitle("Globe Example");
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  UseProgram(GLProgram::GetDefaultTextureProgram());

  GLSimpleRenderablePtr sphere = make_shared<GLSphere>(0.3f, 1024, 512);
  GLImage world;
  world.Load("world.jpg");
  auto texture = make_shared<GLTexture>();
  texture->Texture(world);
  renderable = make_shared<GLTextureDecorator>(sphere, nullptr, texture);
  auto rotation = glm::rotate(mat4(1.0f), -90.0f * deg, vec3(1.0f, 0.0f, 0.0f));
  renderable = make_shared<GLTransformedRenderable>(renderable, rotation);

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
  SetModel(glm::rotate(GetModel(), 2.0f * pi * (GLfloat)dt / 1e10f, vec3(0.0f, 1.0f, 0.0f)));
}
