#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "FileMap.h"
#include "GLApplication.h"
#include "GLFontRange.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLTextureDecorator.h"
#include "Utils.h"

using namespace std;

GL_DECLARE_CLASS(GLPlanarTextureDecorator)

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;
  void Reshape(int w, int h) override;

private:
  GLint imageWidth, imageHeight;
  GLPlanarTextureDecoratorPtr renderable;

  void InitRenderable();
  void UpdateViewer();
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
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  UseProgram(GLProgram::GetDefaultTextureProgram());
  SetModel(scale(mat4(1.0f), vec3(1.0f, -1.0f, 1.0f)));

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
  UpdateViewer();
}

void GLExampleApplication::InitRenderable()
{
  fs::path fontPath = GetFontPath() / "times.ttf";
  GLfloat fontHeight = 64.0f;
  imageWidth = 1024, imageHeight = 1024;
  char32_t firstChar = (char32_t)32, lastChar = (char32_t)126;

  if(argc > 1) {
    fontPath = argv[1];
    if(!fs::exists(fontPath)) fontPath = GetFontPath() / argv[1];
  }
  if(argc > 2) fontHeight = stof(argv[2]);  // sto*: exception thrown on error -> terminate()
  if(argc > 3) imageWidth = stoi(argv[3]);
  if(argc > 4) imageHeight = stoi(argv[4]);
  if(argc > 5) firstChar = (char32_t)stoi(argv[5], nullptr, 0);
  if(argc > 6) lastChar = (char32_t)stoi(argv[6], nullptr, 0);

  if(imageWidth == 0) {
    cerr << "Zero width image: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  }
  if(imageHeight == 0) {
    cerr << "Zero height image: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  }

  GLFontRange fontRange(fontPath, fontHeight, imageWidth, imageHeight, firstChar, lastChar);
  renderable = make_shared<GLPlanarTextureDecorator>(nullptr, nullptr, fontRange.GetTexture());
  UpdateViewer();
}

void GLExampleApplication::UpdateViewer()
{
  GLfloat viewerWidth = 2.0f, viewerHeight = 2.0f;
  GLfloat iWidth = (GLfloat)imageWidth / (GLfloat)GetWindowWidth();
  GLfloat iHeight = (GLfloat)imageHeight / (GLfloat)GetWindowHeight();
  if(iWidth >= iHeight) {
    viewerHeight = iHeight * viewerWidth / iWidth;
  } else {
    viewerWidth = iWidth * viewerHeight / iHeight;
  }
  renderable->SetRenderable(make_shared<GLRectangle>(viewerWidth, viewerHeight));
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
