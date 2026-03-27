#include "FileMap.h"
#include "GLApplication.h"
#include "GLFontRange.h"
#include "GLRenderable.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLFontRange)

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
  GLFontRangeUPtr font;
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
  GLFontRange::Init(GetWindowWidth(), GetWindowHeight());

  fs::path path = GetFontPath() / "times.ttf";
  FileMap fMap(path);
  font = make_unique<GLFontRange>(fMap, fontHeight, atlasWidth, atlasHeight, (wchar_t)32, (wchar_t)126);
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
  GLFontRange::Reshape(w, h);
  InitRenderable();
}

void GLExampleApplication::InitRenderable()
{
  GLfloat x = 50.0f, y = 100.0f;
  wstring words = L"Hello, World!";
  words += L"\n\nPremature optimization is the root of all evil. -- Donald Knuth";
  words += L"\n\n∂ₘFᵐⁿ = μ₀Jⁿ";
  renderable = font->GetRenderable(words, x, y, 50.0f, (GLfloat)GetWindowWidth() - 50.0f);
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
