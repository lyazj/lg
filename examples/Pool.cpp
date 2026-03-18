#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLImage.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLSphere.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "GLTransformedRenderable.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLProgramPtr textureProgram;
  GLCompositeRenderablePtr scene;

  void Frame(uint64_t t, uint64_t dt) override;

  static constexpr GLfloat tableLength = 2.540f;        // m
  static constexpr GLfloat tableWidth = 1.270f;         // m
  static constexpr GLfloat tableBottomHeight = 0.800f;  // m
  static constexpr vec4 tableColor = vec4(0.0f, 77.0f, 57.0f, 255.0f) / 255.0f;
  static constexpr GLfloat ballRadius = 0.028575f;                              // m
  static constexpr GLfloat ballCenterHeight = tableBottomHeight + ballRadius;   // m
  static constexpr GLfloat tableInnerLength = tableLength - 2.0f * ballRadius;  // m
  static constexpr GLfloat tableInnerWidth = tableWidth - 2.0f * ballRadius;    // m

  GLRenderablePtr table;
  vector<GLTransformedRenderablePtr> balls;

  void InitTable();
  void InitBalls();
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
  SetWindowSize(1024, 512);
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();

  vec3 camera(0.0f, 3.0f, 0.0f);  // m
  vec3 target(0.0f, 0.0f, 0.0f);
  vec3 up(0.0f, 0.0f, -1.0f);
  SetView(glm::lookAt(camera, target, up));
  SetProjection(45.0f * deg, 0.1f, 10.0f);  // m

  UseProgram(GLProgram::GetDefaultProgram());
  textureProgram = GLProgram::GetDefaultTextureProgram();

  scene = make_shared<GLCompositeRenderable>();
  InitTable();
  InitBalls();
  scene->SetVertexAttributes();
  scene->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  scene->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt [[maybe_unused]])
{
  // [TODO]
}

void GLExampleApplication::InitTable()
{
  GLSimpleRenderablePtr t = make_shared<GLRectangle>(tableLength, tableWidth);
  t = make_shared<GLUniformColorDecorator>(t, tableColor);
  auto m = mat4(1.0f);
  m = glm::translate(m, vec3(0.0f, tableBottomHeight, 0.0f));
  m = glm::rotate(m, -90.0f * deg, vec3(1.0f, 0.0f, 0.0f));
  table = make_shared<GLTransformedRenderable>(t, m);
  scene->AddGeometry(table);
}

void GLExampleApplication::InitBalls()
{
  auto ball = make_shared<GLSphere>(ballRadius, 64, 32);  // m
  for(int i = 0; i < 16; ++i) {
    GLImage ballImage;
    string suffix = i == 0 ? "cue" : to_string(i);
    ballImage.Load(GetTexturePath() / "pool" / ("ball-" + suffix + ".jpg"));
    auto texture = make_shared<GLTexture>();
    texture->Texture(ballImage);
    auto thisBall = make_shared<GLTextureDecorator>(ball, textureProgram, texture);

    auto translation = vec3{
      (RandFloat() - 0.5f) * tableInnerLength,
      ballCenterHeight,
      (RandFloat() - 0.5f) * tableInnerWidth,
    };
    auto transform = glm::mat4(1.0f);
    transform = glm::translate(transform, translation);
    transform *= RandomRotation();
    balls.push_back(make_shared<GLTransformedRenderable>(thisBall, transform));
    scene->AddGeometry(balls.back());
  }
}
