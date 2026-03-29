#include "GLApplication.h"
#include "GLCompositeRenderable.h"
#include "GLFont.h"
#include "GLRenderable.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLFont)

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;
  void Reshape(int w, int h) override;

private:
  GLFontUPtr latinFont;
  GLFontUPtr chineseFont;
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

  latinFont = make_unique<GLFont>(GetFontPath() / "times.ttf", 48.0f);
  chineseFont = make_unique<GLFont>(GetFontPath() / "simkai.ttf", 48.0f);
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
  InitRenderable();
}

void GLExampleApplication::InitRenderable()
{
  GLCompositeRenderablePtr texts = make_shared<GLCompositeRenderable>();
  GLfloat x = 50.0f, y = 100.0f;

  wstring words = L"Hello, World!";
  words += L"\n\nPremature optimization is the root of all evil. — Donald Knuth";
  words += L"\n\n∂ₘFᵐⁿ = μ₀Jⁿ. — Maxwell";
  texts->AddRenderable(latinFont->GetRenderable(words, x, y, 50.0f, (GLfloat)GetWindowWidth() - 50.0f));

  words = L"\n\n思想自由，兼容并包。——蔡元培";
  texts->AddRenderable(chineseFont->GetRenderable(words, x, y, 50.0f, (GLfloat)GetWindowWidth() - 50.0f));

  words = LoadUTF8(GetTextPath() / "world.txt");
  while(!words.empty() && words.back() == L'\n') words.pop_back();
  words = L"\n\n" + words;
  texts->AddRenderable(chineseFont->GetRenderable(words, x, y, 50.0f, (GLfloat)GetWindowWidth() - 50.0f));

  renderable = texts;
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
