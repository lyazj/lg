#include <iostream>

#include "GLApplication.h"
#include "GLImage.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;
  void Reshape(int w, int h) override;

private:
  GLint imageWidth, imageHeight;
  shared_ptr<GL2DTextureDecorator> renderable;

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

  SetTitle("Image Viewer Example");
  SetFrameRate(0);
  SetShowFrameRateInterval(0);
}

void GLExampleApplication::Init()
{
  GLApplication::Init();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  UseProgram(GLProgram::GetDefaultTextureProgram());

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
  if(argc != 2) {
    cerr << "Usage: " << GetProgramShortName() << " <image>" << endl;
    exit(EXIT_FAILURE);
  }

  GLImage image;
  image.Load(argv[1]);
  imageWidth = image.GetWidth(), imageHeight = image.GetHeight();
  if(imageWidth == 0) {
    cerr << "Zero width image: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  }
  if(imageHeight == 0) {
    cerr << "Zero height image: " << argv[1] << endl;
    exit(EXIT_FAILURE);
  }

  renderable = make_shared<GL2DTextureDecorator>(nullptr, nullptr, make_shared<GLTexture>());
  renderable->GetTexture()->Texture(image);
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
  renderable->SetGeometry(make_shared<GLRectangle>(viewerWidth, viewerHeight));
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
