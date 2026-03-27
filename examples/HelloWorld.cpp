#include "GLApplication.h"
#include "GLFont.h"
#include "GLRenderable.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;
  void Reshape(int w, int h) override;

private:
  GLfloat fontHeight = 64.0f;
  GLint atlasWidth = 1024, atlasHeight = 1024;
  GLRenderablePtr renderable;

  void InitRenderable();
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
  GLApplication::Init();
  EnableBlend();
  GLFont::Init(GetWindowWidth(), GetWindowHeight());
  InitRenderable();
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Reshape(int w, int h)
{
  GLApplication::Reshape(w, h);
  GLFont::Reshape(w, h);
}

void GLExampleApplication::InitRenderable()
{
  fs::path path = GetFontPath() / "times.ttf";
  GLFont font(path, fontHeight, atlasWidth, atlasHeight, 32, 95);
  GLfloat x = 50.0f, y = 100.0f;
  renderable = font.GetRenderable("Hello, World!", x, y, 50.0f, (GLfloat)GetWindowWidth() - 50.0f);
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
