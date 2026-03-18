#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLCircle.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLImage.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLSphere.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "GLTransformedRenderable.h"
#include "GLTriangleStrip.h"
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

  static constexpr GLfloat tableInnerLength = 2.540f;  // m
  static constexpr GLfloat tableInnerWidth = 1.270f;   // m
  static constexpr GLfloat tableTopHeight = 0.840f;    // m

  static constexpr GLfloat borderHeight = 0.042f;  // m [XXX] Not implemented.
  static constexpr GLfloat borderWidth = 0.085f;   // m

  static constexpr GLfloat tableBottomHeight = tableTopHeight - borderHeight;         // m
  static constexpr GLfloat tableOuterLength = tableInnerLength + 2.0f * borderWidth;  // m
  static constexpr GLfloat tableOuterWidth = tableInnerWidth + 2.0f * borderWidth;    // m

  static constexpr vec4 tableColor = vec4(34.0f, 139.0f, 34.0f, 255.0f) / 255.0f;
  static constexpr vec4 borderColor = vec4(40.0f, 30.0f, 20.0f, 255.0f) / 255.0f;
  static constexpr vec4 holeColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);

  static constexpr GLfloat ballRadius = 0.028575f;                                 // m
  static constexpr GLfloat ballAreaLength = tableInnerLength - 2.0f * ballRadius;  // m
  static constexpr GLfloat ballAreaWidth = tableInnerWidth - 2.0f * ballRadius;    // m
  static constexpr GLfloat ballAreaHeight = tableBottomHeight + ballRadius;        // m

  static constexpr GLfloat tableLengthAvg = (tableInnerLength + tableOuterLength) * 0.5f;
  static constexpr GLfloat tableWidthAvg = (tableInnerWidth + tableOuterWidth) * 0.5f;
  static constexpr GLfloat holeOffset = borderWidth / 2.8284271247461903f;
  static constexpr vec2 holePositions[6] = {
    { -tableLengthAvg * 0.5f + holeOffset, +tableWidthAvg * 0.5f - holeOffset },
    { +tableLengthAvg * 0.0f, +tableWidthAvg * 0.5f },
    { +tableLengthAvg * 0.5f - holeOffset, +tableWidthAvg * 0.5f - holeOffset },
    { -tableLengthAvg * 0.5f + holeOffset, -tableWidthAvg * 0.5f + holeOffset },
    { +tableLengthAvg * 0.0f, -tableWidthAvg * 0.5f },
    { +tableLengthAvg * 0.5f - holeOffset, -tableWidthAvg * 0.5f + holeOffset },
  };

  static constexpr GLfloat frictionDeceleration = 0.3f;  // m/s^2
  static constexpr GLfloat elasticityBallBall = 0.9f;
  static constexpr GLfloat elasticityBallBorder = 0.8f;

  static constexpr GLfloat maxInitialVelocity = 3.0f;  // m/s
  static constexpr GLfloat minVelocity = 1e-4f;        // m/s
  static constexpr GLfloat minRotation = 1e-4f;        // rad

  vector<vec2> ballPositions;
  vector<vec2> ballVelocities;
  vector<GLTransformedRenderablePtr> balls;

  void InitTable();
  void InitBalls();

  GLint collisionBall = -1;
  GLint collisionType = -1;

  GLfloat GetFreeTime(GLfloat dt);
  void Transport(GLfloat dt);
  void HandleCollisions();

  void RegularizeVelocity(vec2 &v) const;
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

  vec3 camera(0.0f, 0.0f, 3.0f);  // m
  vec3 target(0.0f, 0.0f, 0.0f);
  vec3 up(0.0f, 1.0f, 0.0f);
  SetView(lookAt(camera, target, up));
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

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt_in [[maybe_unused]])
{
  GLfloat dt = (GLfloat)dt_in * 1e-9f;  // s
  while(dt > 0.0f) {
    GLfloat ft = GetFreeTime(dt);
    Transport(ft);
    HandleCollisions();
    dt -= ft;
  }
}

void GLExampleApplication::InitTable()
{
  auto table0 = make_shared<GLCompositeRenderable>();
  mat4 transform;

  // Table surface.
  GLSimpleRenderablePtr t0 = make_shared<GLRectangle>(tableOuterLength, tableOuterWidth);
  auto t1 = make_shared<GLUniformColorDecorator>(t0, tableColor);
  table0->AddGeometry(t1);

  // Horizontal borders.
  auto b0 = make_shared<GLTriangleStrip>();
  b0->Reserve(4);
  b0->AddVertex({ -tableOuterLength * 0.5f, +tableOuterWidth * 0.5f, 0.001f });
  b0->AddVertex({ -tableInnerLength * 0.5f + 2.0f * ballRadius, +tableInnerWidth * 0.5f, 0.001f });
  b0->AddVertex({ 0.0f, +tableOuterWidth * 0.5f, 0.001f });
  b0->AddVertex({ 0.0f - 2.0f * ballRadius, +tableInnerWidth * 0.5f, 0.001f });
  auto b1 = make_shared<GLUniformColorDecorator>(b0, borderColor);
  table0->AddGeometry(b1);
  transform = mat4(1.0f);
  transform[0][0] = -1.0f;
  table0->AddGeometry(make_shared<GLTransformedRenderable>(b1, transform));
  transform[1][1] = -1.0f;
  table0->AddGeometry(make_shared<GLTransformedRenderable>(b1, transform));
  transform[0][0] = +1.0f;
  table0->AddGeometry(make_shared<GLTransformedRenderable>(b1, transform));

  // Vertical borders.
  auto b2 = make_shared<GLTriangleStrip>();
  b2->Reserve(4);
  b2->AddVertex({ -tableOuterLength * 0.5f, +tableOuterWidth * 0.5f, 0.001f });
  b2->AddVertex({ -tableInnerLength * 0.5f, +tableInnerWidth * 0.5f - 2.0f * ballRadius, 0.001f });
  b2->AddVertex({ -tableOuterLength * 0.5f, -tableOuterWidth * 0.5f, 0.001f });
  b2->AddVertex({ -tableInnerLength * 0.5f, -tableInnerWidth * 0.5f + 2.0f * ballRadius, 0.001f });
  auto b3 = make_shared<GLUniformColorDecorator>(b2, borderColor);
  table0->AddGeometry(b3);
  transform = mat4(1.0f);
  transform[0][0] = -1.0f;
  table0->AddGeometry(make_shared<GLTransformedRenderable>(b3, transform));

  // Holes.
  auto h0 = make_shared<GLCircle>(borderWidth * 0.5f, 64);
  auto h1 = make_shared<GLUniformColorDecorator>(h0, holeColor);
  for(GLint i = 0; i < 6; ++i) {
    transform = translate(mat4(1.0f), vec3(holePositions[i], 0.002f));
    table0->AddGeometry(make_shared<GLTransformedRenderable>(h1, transform));
  }

  // Translate: table surface -> table bottom.
  transform = translate(mat4(1.0f), vec3(0.0f, 0.0f, tableBottomHeight));
  auto table1 = make_shared<GLTransformedRenderable>(table0, transform);

  scene->AddGeometry(table1);
}

void GLExampleApplication::InitBalls()
{
  ballPositions.reserve(16);
  ballVelocities.reserve(16);
  balls.reserve(16);

  auto ball = make_shared<GLSphere>(ballRadius, 64, 32);
  for(GLint i = 0; i < 16; ++i) {
    // Texture depicting number and pattern.
    GLImage ballImage;
    string suffix = i == 0 ? "cue" : to_string(i);
    ballImage.Load(GetTexturePath() / "pool" / ("ball-" + suffix + ".jpg"));
    auto texture = make_shared<GLTexture>();
    texture->Texture(ballImage);
    auto thisBall = make_shared<GLTextureDecorator>(ball, textureProgram, texture);

    // Randomize initial position, velocity, and rotation.
    ballPositions.emplace_back((RandFloat() - 0.5f) * ballAreaLength, (RandFloat() - 0.5f) * ballAreaWidth);
    ballVelocities.emplace_back(maxInitialVelocity * RandFloat() * RandDirection2D());
    auto translation = vec3(ballPositions.back(), ballAreaHeight);
    auto transform = translate(mat4(1.0f), translation) * RandRotation3D();
    balls.push_back(make_shared<GLTransformedRenderable>(thisBall, transform));
    scene->AddGeometry(balls.back());
  }
}

GLfloat GLExampleApplication::GetFreeTime(GLfloat dt)
{
  for(GLint i = 0; i < (GLint)balls.size(); ++i) {
    vec2 x = ballPositions[i], v = ballVelocities[i];
    if(length(v) == 0.0f) continue;

    GLfloat ft = min(ballRadius / length(v), length(v) / frictionDeceleration);
    if(ft < dt) dt = ft, collisionBall = i, collisionType = 0;  // Step limiter.

    if(v.x < 0.0f) {
      ft = (x.x + ballAreaLength * 0.5f) / -v.x;
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 1;  // Left border.
    } else if(v.x > 0.0f) {
      ft = (ballAreaLength * 0.5f - x.x) / v.x;
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 2;  // Right border.
    }

    if(v.y < 0.0f) {
      ft = (x.y + ballAreaWidth * 0.5f) / -v.y;
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 3;  // Bottom border.
    } else if(v.y > 0.0f) {
      ft = (ballAreaWidth * 0.5f - x.y) / v.y;
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 4;  // Top border.
    }
  }

  return dt;
}

void GLExampleApplication::Transport(GLfloat dt)
{
  for(GLint i = 0; i < (GLint)balls.size(); ++i) {
    if(length(ballVelocities[i]) == 0.0f) continue;

    // Assume pure rolling.
    vec2 newVelocity = ballVelocities[i] - frictionDeceleration * dt * normalize(ballVelocities[i]);
    vec2 displacement = (ballVelocities[i] + newVelocity) * 0.5f * dt;
    vec3 theta = cross(vec3(0.0f, 0.0f, 1.0f), vec3(displacement, 0.0f)) / ballRadius;

    // Update data.
    ballPositions[i] += displacement;
    ballVelocities[i] = newVelocity;
    RegularizeVelocity(ballVelocities[i]);
    mat4 transform = translate(mat4(1.0f), vec3(ballPositions[i], ballAreaHeight));
    if(length(theta) > minRotation) transform *= rotate(mat4(1.0f), length(theta), normalize(theta));
    transform *= mat4(mat3(balls[i]->GetModel()));
    balls[i]->SetModel(transform);
  }
}

static GLfloat GetBorderBouncingVelocity(GLfloat v, GLfloat elasticity)
{
  return GLfloat(1 - 2 * !!signbit(v)) * sqrtf(v * v * elasticity);
}

void GLExampleApplication::HandleCollisions()
{
  switch(collisionType) {
  case -1:  // No collision.
    return;
  case 0:  // Step limiter.
    break;
  case 1:  // Left border.
  case 2:  // Right border.
    ballVelocities[collisionBall].x = -GetBorderBouncingVelocity(ballVelocities[collisionBall].x, elasticityBallBorder);
    break;
  case 3:  // Bottom border.
  case 4:  // Top border.
    ballVelocities[collisionBall].y = -GetBorderBouncingVelocity(ballVelocities[collisionBall].y, elasticityBallBorder);
    break;
  default: abort();
  }

  RegularizeVelocity(ballVelocities[collisionBall]);
  collisionBall = -1;
  collisionType = -1;
}

void GLExampleApplication::RegularizeVelocity(vec2 &v) const
{
  for(GLfloat *p : { &v.x, &v.y }) {
    if(fabsf(*p) < minVelocity) *p = 0.0f;
  }
}
