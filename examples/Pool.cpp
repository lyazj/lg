#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

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

  static constexpr GLfloat borderHeight = 0.042f;  // m; not used for top-down perspective
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
  static constexpr GLfloat holeRadius = borderWidth * 0.5f;
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
  static constexpr GLfloat minDistance = 1e-4f;        // m
  static constexpr GLfloat minRotation = 1e-4f;        // rad
  static constexpr GLfloat minTime = 1e-4f;            // s

  vector<vec2> ballPositions;
  vector<vec2> ballVelocities;
  GLCompositeRenderablePtr balls;        // each component of type GLTransformedRenderable
  vector<GLint> ballApproachingHoles;    // 0--5: approaching the hole; -1: not; -2: holed
  vector<pair<GLint, GLint>> ballPairs;  // sorted by increasing ball-ball distance
  GLint nGoals = 0;

  void InitTable();
  void InitBalls();

  GLint collisionBall = -1;
  GLint collisionType = -1;

  GLfloat GetFreeTime(GLfloat dt);
  void Transport(GLfloat dt);
  void HandleCollision();
  void UpdateApproachingHole(GLint i);
  void UpdateBallPairs();

  void RegularizeVelocity(vec2 &v) const;  // Use it after each change.
  GLfloat RegularizeDistance(GLfloat d) const;
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
  MaximizeWindow();

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
    HandleCollision();
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
  table0->AddRenderable(t1);

  // Horizontal borders.
  auto b0 = make_shared<GLTriangleStrip>();
  b0->Reserve(4);
  b0->AddVertex({ -tableOuterLength * 0.5f, +tableOuterWidth * 0.5f, 0.001f });
  b0->AddVertex({ -tableInnerLength * 0.5f + 2.0f * ballRadius, +tableInnerWidth * 0.5f, 0.001f });
  b0->AddVertex({ 0.0f, +tableOuterWidth * 0.5f, 0.001f });
  b0->AddVertex({ 0.0f - 2.0f * ballRadius, +tableInnerWidth * 0.5f, 0.001f });
  auto b1 = make_shared<GLUniformColorDecorator>(b0, borderColor);
  table0->AddRenderable(b1);
  transform = mat4(1.0f);
  transform[0][0] = -1.0f;
  table0->AddRenderable(make_shared<GLTransformedRenderable>(b1, transform));
  transform[1][1] = -1.0f;
  table0->AddRenderable(make_shared<GLTransformedRenderable>(b1, transform));
  transform[0][0] = +1.0f;
  table0->AddRenderable(make_shared<GLTransformedRenderable>(b1, transform));

  // Vertical borders.
  auto b2 = make_shared<GLTriangleStrip>();
  b2->Reserve(4);
  b2->AddVertex({ -tableOuterLength * 0.5f, +tableOuterWidth * 0.5f, 0.001f });
  b2->AddVertex({ -tableInnerLength * 0.5f, +tableInnerWidth * 0.5f - 2.0f * ballRadius, 0.001f });
  b2->AddVertex({ -tableOuterLength * 0.5f, -tableOuterWidth * 0.5f, 0.001f });
  b2->AddVertex({ -tableInnerLength * 0.5f, -tableInnerWidth * 0.5f + 2.0f * ballRadius, 0.001f });
  auto b3 = make_shared<GLUniformColorDecorator>(b2, borderColor);
  table0->AddRenderable(b3);
  transform = mat4(1.0f);
  transform[0][0] = -1.0f;
  table0->AddRenderable(make_shared<GLTransformedRenderable>(b3, transform));

  // Holes.
  auto h0 = make_shared<GLCircle>(holeRadius, 64);
  auto h1 = make_shared<GLUniformColorDecorator>(h0, holeColor);
  for(GLint i = 0; i < 6; ++i) {
    transform = translate(mat4(1.0f), vec3(holePositions[i], 0.002f));
    table0->AddRenderable(make_shared<GLTransformedRenderable>(h1, transform));
  }

  // Translate: table surface -> table bottom.
  transform = translate(mat4(1.0f), vec3(0.0f, 0.0f, tableBottomHeight));
  auto table1 = make_shared<GLTransformedRenderable>(table0, transform);

  scene->AddRenderable(table1);
}

void GLExampleApplication::InitBalls()
{
  ballPositions.reserve(16);
  ballVelocities.reserve(16);
  balls = make_shared<GLCompositeRenderable>();
  balls->Reserve(16);

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
    vec2 position;
    for(;;) {
      position = { (RandFloat() - 0.5f) * ballAreaLength, (RandFloat() - 0.5f) * ballAreaWidth };
      bool overlap = false;
      for(const auto &p : ballPositions) {
        if(length(position - p) < 2.0f * ballRadius) {
          overlap = true;
          break;
        }
      }
      if(overlap) continue;
      break;
    }
    ballPositions.emplace_back(position);
    ballVelocities.emplace_back(maxInitialVelocity * RandFloat() * RandDirection2D());
    auto translation = vec3(ballPositions.back(), ballAreaHeight);
    auto transform = translate(mat4(1.0f), translation) * RandRotation3D();
    balls->AddRenderable(make_shared<GLTransformedRenderable>(thisBall, transform));
    ballApproachingHoles.push_back(-1);
    for(GLint j = 0; j < (GLint)balls->GetNRenderable() - 1; ++j) {
      ballPairs.emplace_back(j, balls->GetNRenderable() - 1);
    }
    UpdateApproachingHole(GLint(balls->GetNRenderable() - 1));
  }
  UpdateBallPairs();

  scene->AddRenderable(balls);
}

// Compute the time before a ball-border collision.
static GLfloat SolveTime(GLfloat x, GLfloat v, GLfloat a)
{
  // at^2/2 - vt + x = 0, a > 0, v > 0, x > 0
  GLfloat d = v * v - 2.0f * a * x;
  if(d < 0.0f) return INFINITY;
  return (v - sqrtf(d)) / a;
}

// Compute the time until a ball-ball collision.
// The parameter tmax satisfies invariants (1)–(4) listed below.
// Since invariant (5) is not established, the computed time may be
// invalidated by an earlier ball-ball collision. However, that
// collision will override this result, so this is safe.
static GLfloat SolveTime(vec2 x, vec2 v, vec2 a, GLfloat r, GLfloat tmax, GLfloat minDistance)
{
  // Trajectory: x(t) = x + vt - at^2/2
  //
  // (1) Early exit if tmax is too small.
  // (2) Solve |x(t)| = 2r.
  // (3) Return the smallest viable solution or INFINITY.

  // (1)
  // |x(t)| >= |x(0)| - |vt - at^2/2| >= |x(0)| - (|v|t + |a|t^2/2)
  // So if |x(0)| - (|v|t + |a|t^2/2) > 2r, or equivalently |v|t + |a|t^2/2 < |x(0)| - 2r, no collision can happen.
  if(length(v) * tmax + length(a) * tmax * tmax * 0.5f < length(x) - 2.0f * r) return INFINITY;

  // (2)
  // Solve f(t) = |x(t)|^2 - 4r^2 = 0
  complex<double> t_f[4];
  double t_f_a[5] = {
    0.25f * dot(a, a),
    -dot(a, v),
    dot(v, v) - dot(a, x),
    2.0f * dot(v, x),
    dot(x, x) - 4.0f * r * r,
  };
  GLint nSolution = SolveQuartic(t_f, t_f_a);
  if(nSolution == -1) {  // failed
    cerr << "Error: SolveQuartic failed: a = " << t_f_a[0] << ", b = " << t_f_a[1] << ", c = " << t_f_a[2]
         << ", d = " << t_f_a[3] << ", e = " << t_f_a[4] << endl;
    Debug();
  }
  nSolution = abs(nSolution);  // 0: no solution or infinite solutions; -4: biquadratic

  // (3)
  // Pick the smallest viable solution.
  // Use f'(t) to determine approaching/receding.
  for(GLint i = 0; i < nSolution; ++i) {
    double residual = abs(t_f_a[0] * t_f[i] * t_f[i] * t_f[i] * t_f[i] + t_f_a[1] * t_f[i] * t_f[i] * t_f[i]
        + t_f_a[2] * t_f[i] * t_f[i] + t_f_a[3] * t_f[i] + t_f_a[4]);
    if(!(residual <= minDistance * minDistance)) {  // including nan
      if(!(abs(t_f[i].imag()) >= minDistance)) {    // negligible imagine part; including nan
        cerr << "Warning: SolveQuartic solution " << i << " has large residual: t = " << t_f[i]
             << ", residual = " << residual << endl;
      }
    }
    GLfloat t = (GLfloat)t_f[i].real();
    if(t < 0 || t > tmax) continue;  // Out of bound.
    GLfloat dist = length(x + v * t - 0.5f * a * t * t) - 2.0f * r;
    if(dist > minDistance) continue;  // Not real.
    GLfloat fpt = GLfloat(4.0f * t_f_a[0] * t * t * t + 3.0f * t_f_a[1] * t * t + 2.0f * t_f_a[2] * t + t_f_a[3]);
    if(fpt >= 0.0f) continue;  // Receding.
    tmax = min(tmax, t);
  }
  return tmax;
}

GLfloat GLExampleApplication::GetFreeTime(GLfloat dt)
{
  // These states are also cleared in HandleCollision(),
  // but we reset them here for clarity and robustness.
  collisionBall = -1;
  collisionType = -1;

  // Invariants in [0, dt):
  // (1) No ball-border collision occurs.
  // (2) No ball enters a hole.
  // (3) No ball travels a distance greater than its radius.
  // (4) No moving ball comes to a stop.
  for(GLint i = 0; i < (GLint)balls->GetNRenderable(); ++i) {
    vec2 xi = ballPositions[i], vi = ballVelocities[i];
    if(length(vi) == 0.0f) continue;

    GLfloat maxLength = ballRadius;
    GLint hole = ballApproachingHoles[i];
    if(hole >= 0) maxLength = min(maxLength, length(holePositions[hole] - xi));
    GLfloat ft = min(maxLength / length(vi), length(vi) / frictionDeceleration);
    if(ft < dt) dt = ft, collisionBall = i, collisionType = 0;  // Step limiter.
    if(hole >= 0) continue;  // No border collisions if the ball is approaching a hole.

    if(vi.x < 0.0f) {
      ft = SolveTime(xi.x + ballAreaLength * 0.5f, -vi.x, frictionDeceleration);
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 1;  // Left border.
    } else if(vi.x > 0.0f) {
      ft = SolveTime(ballAreaLength * 0.5f - xi.x, vi.x, frictionDeceleration);
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 2;  // Right border.
    }

    if(vi.y < 0.0f) {
      ft = SolveTime(xi.y + ballAreaWidth * 0.5f, -vi.y, frictionDeceleration);
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 3;  // Bottom border.
    } else if(vi.y > 0.0f) {
      ft = SolveTime(ballAreaWidth * 0.5f - xi.y, vi.y, frictionDeceleration);
      if(ft < dt) dt = ft, collisionBall = i, collisionType = 4;  // Top border.
    }
  }

  // (5) No ball-ball collision happens.
  // Heuristic: ballPairs is sorted by increasing ball-ball distance.
  // This allows us to quickly narrow the valid dt range and early-exit
  // for pairs that are too far apart when solving the equations.
  for(auto [i, j] : ballPairs) {
    vec2 xi = ballPositions[i], vi = ballVelocities[i];
    vec2 ai = length(vi) != 0.0f ? -frictionDeceleration * normalize(vi) : vec2(0.0f);
    vec2 xj = ballPositions[j], vj = ballVelocities[j];
    vec2 aj = length(vj) != 0.0f ? -frictionDeceleration * normalize(vj) : vec2(0.0f);
    vec2 x = xi - xj, v = vi - vj, a = ai - aj;
    GLfloat ft = SolveTime(x, v, -a, ballRadius, dt, minDistance);
    if(ft < dt) dt = ft, collisionBall = i, collisionType = 16 + j;  // Ball-ball collision.
  }

  // The invariants above ensure that each ball moves in a straight line
  // and decelerates uniformly due to table friction.
  return dt;
}

void GLExampleApplication::Transport(GLfloat dt)
{
  for(GLint i = 0; i < (GLint)balls->GetNRenderable(); ++i) {
    if(length(ballVelocities[i]) == 0.0f) continue;

    // Assume pure rolling.
    vec2 newVelocity = ballVelocities[i] - frictionDeceleration * dt * normalize(ballVelocities[i]);
    vec2 displacement = (ballVelocities[i] + newVelocity) * 0.5f * dt;
    vec3 theta = cross(vec3(0.0f, 0.0f, 1.0f), vec3(displacement, 0.0f)) / ballRadius;

    // Update data.
    ballPositions[i] += displacement;
    ballVelocities[i] = newVelocity;
    RegularizeVelocity(ballVelocities[i]);
    UpdateApproachingHole(i);

    // Update model.
    mat4 transform = translate(mat4(1.0f), vec3(ballPositions[i], ballAreaHeight));
    if(length(theta) > minRotation) transform *= rotate(mat4(1.0f), length(theta), normalize(theta));
    GLTransformedRenderable *ball = (GLTransformedRenderable *)balls->GetRenderable(i).get();
    transform *= mat4(mat3(ball->GetModel()));
    ball->SetModel(transform);
  }

  UpdateBallPairs();
}

static GLfloat GetBouncingVelocity(GLfloat v, GLfloat elasticity)
{
  return GLfloat(1 - 2 * !!signbit(v)) * sqrtf(v * v * elasticity);
}

// Compute the velocities after a ball-ball collision. Assume equal mass and no friction.
static void HandleCollision(const vec2 &x0, vec2 &v0, const vec2 &x1, vec2 &v1, GLfloat elasticity)
{
  vec2 x = normalize(x1 - x0), v = v1 - v0, pcom = v0 + v1;  // m1 = m2 = 1
  GLfloat vn = dot(v, x);
  vec2 vt_v = v - vn * x;
  vn = -GetBouncingVelocity(vn, elasticity);
  v = vt_v + vn * x;
  v0 = (-v + pcom) * 0.5f;
  v1 = (+v + pcom) * 0.5f;
}

void GLExampleApplication::HandleCollision()
{
  if(collisionType == -1) return;  // No collision.
  if(collisionType == 0) {         // Step limiter.
    collisionType = -1;
    return;
  }
  if(ballApproachingHoles[collisionBall] == -2) {  // Already in hole.
    collisionType = -1;
    return;
  }

  if(collisionType < 16) {                          // Ball-border collision.
    if(ballApproachingHoles[collisionBall] >= 0) {  // No border collision if the ball is approaching a hole.
      collisionType = -1;
      return;
    }
    switch(collisionType) {
    case 1:  // Left border.
      //clog << "Info: distance to the left border for ball " << collisionBall << ": "
      //     << RegularizeDistance(ballPositions[collisionBall].x + ballAreaLength * 0.5f) << endl;
      ballVelocities[collisionBall].x = -GetBouncingVelocity(ballVelocities[collisionBall].x, elasticityBallBorder);
      break;
    case 2:  // Right border.
      //clog << "Info: distance to the right border for ball " << collisionBall << ": "
      //     << RegularizeDistance(ballAreaLength * 0.5f - ballPositions[collisionBall].x) << endl;
      ballVelocities[collisionBall].x = -GetBouncingVelocity(ballVelocities[collisionBall].x, elasticityBallBorder);
      break;
    case 3:  // Bottom border.
      //clog << "Info: distance to the bottom border for ball " << collisionBall << ": "
      //     << RegularizeDistance(ballPositions[collisionBall].y + ballAreaWidth * 0.5f) << endl;
      ballVelocities[collisionBall].y = -GetBouncingVelocity(ballVelocities[collisionBall].y, elasticityBallBorder);
      break;
    case 4:  // Top border.
      //clog << "Info: distance to the top border for ball " << collisionBall << ": "
      //     << RegularizeDistance(ballAreaWidth * 0.5f - ballPositions[collisionBall].y) << endl;
      ballVelocities[collisionBall].y = -GetBouncingVelocity(ballVelocities[collisionBall].y, elasticityBallBorder);
      break;
    default: break;
    }
  }

  else {  // Ball-ball collision.
    GLint i0 = collisionBall, i1 = collisionType - 16;
    if(ballApproachingHoles[i1] == -2) {  // Already in hole.
      collisionType = -1;
      return;
    }
    //clog << "Info: ball-ball collision between " << i0 << " and " << i1
    //     << ", distance: " << RegularizeDistance(length(ballPositions[i1] - ballPositions[i0]) - 2.0f * ballRadius)
    //     << ", relative velocity: " << ballVelocities[i1] - ballVelocities[i0] << endl;
    ::HandleCollision(ballPositions[i0], ballVelocities[i0], ballPositions[i1], ballVelocities[i1], elasticityBallBall);
    RegularizeVelocity(ballVelocities[i1]);
    UpdateApproachingHole(i1);
  }

  RegularizeVelocity(ballVelocities[collisionBall]);
  UpdateApproachingHole(collisionBall);
  collisionBall = -1;
  collisionType = -1;
}

// State machine (move → approach → enter) for each hole.
// The border near the hole is approximated as a "magic" capture based on position and velocity.
// This is visually acceptable and can be refined in the future.
void GLExampleApplication::UpdateApproachingHole(GLint i)
{
  GLint iHole = ballApproachingHoles[i];
  if(iHole == -2) return;  // Already in hole.
  if(iHole == -1) {        // Not approaching any hole yet.
    GLfloat x = ballPositions[i].x, y = ballPositions[i].y, vx = ballVelocities[i].x, vy = ballVelocities[i].y;
    if(x + ballAreaLength * 0.5f < ballRadius && ballAreaWidth * 0.5f - y < ballRadius && vx < 0.0f && vy > 0.0f) {
      iHole = 0;  // Top-left hole.
    }
    if(fabs(x) < ballRadius && ballAreaWidth * 0.5f - y < ballRadius && vy > 0.0f) {
      iHole = 1;  // Top-middle hole.
    }
    if(ballAreaLength * 0.5f - x < ballRadius && ballAreaWidth * 0.5f - y < ballRadius && vx > 0.0f && vy > 0.0f) {
      iHole = 2;  // Top-right hole.
    }
    if(x + ballAreaLength * 0.5f < ballRadius && y + ballAreaWidth * 0.5f < ballRadius && vx < 0.0f && vy < 0.0f) {
      iHole = 3;  // Bottom-left hole.
    }
    if(fabs(x) < ballRadius && y + ballAreaWidth * 0.5f < ballRadius && vy < 0.0f) {
      iHole = 4;  // Bottom-middle hole.
    }
    if(ballAreaLength * 0.5f - x < ballRadius && y + ballAreaWidth * 0.5f < ballRadius && vx > 0.0f && vy < 0.0f) {
      iHole = 5;  // Bottom-right hole.
    }
    if(iHole < 0) return;
    // move → approach: the "magic" capture
    ballVelocities[i] = length(ballVelocities[i]) * normalize(holePositions[iHole] - ballPositions[i]);
  }
  if(length(holePositions[iHole] - ballPositions[i]) < holeRadius) {  // Entering the hole.
    // approach -> enter: move the ball out from the table
    iHole = -2;
    ballPositions[i] = {
      (GLfloat)(nGoals - 7) * 3.0f * ballRadius,
      tableOuterWidth * 0.5f + 2.0f * ballRadius,
    };
    ballVelocities[i] = { 0.0f, 0.0f };
    mat4 transform = translate(mat4(1.0f), vec3(ballPositions[i], ballAreaHeight));
    transform = rotate(transform, -75.0f * deg, vec3(1.0f, 0.0f, 0.0f));
    transform = rotate(transform, +90.0f * deg, vec3(0.0f, 0.0f, 1.0f));
    transform = scale(transform, vec3(1.2f, 1.2f, 1.2f));
    ((GLTransformedRenderable *)balls->GetRenderable(i).get())->SetModel(transform);
    ++nGoals;
    UpdateBallPairs();
  }
  ballApproachingHoles[i] = iHole;
}

// Sort ball pairs by increasing distance.
// Remove pairs involving balls that are already in holes.
void GLExampleApplication::UpdateBallPairs()
{
  auto less = [&bp = ballPositions](pair<GLint, GLint> a, pair<GLint, GLint> b) {
    auto [a0, a1] = a;
    auto [b0, b1] = b;
    return length(bp[a0] - bp[a1]) < length(bp[b0] - bp[b1]);
  };

  // The sequence is nearly sorted. Use insertion sort for better performance.
  for(GLint i = 0; i < (GLint)ballPairs.size(); ++i) {  // length of sorted prefix
    // Removal changes the order, but it's very rare and has little impact on performance.
    while(i < (GLint)ballPairs.size()) {
      auto [b0, b1] = ballPairs[i];
      if(ballApproachingHoles[b0] != -2 && ballApproachingHoles[b1] != -2) break;
      ballPairs[i] = ballPairs.back();
      ballPairs.pop_back();
    }
    if(i == (GLint)ballPairs.size()) break;

    GLint j = i;  // The position to be inserted into.
    pair<GLint, GLint> cur = ballPairs[i];
    while(j > 0 && less(cur, ballPairs[j - 1])) {
      ballPairs[j] = ballPairs[j - 1];
      --j;
    }
    ballPairs[j] = cur;
  }
}

void GLExampleApplication::RegularizeVelocity(vec2 &v) const
{
  for(GLfloat *p : { &v.x, &v.y }) {
    if(fabsf(*p) < minVelocity) *p = 0.0f;
  }
}

GLfloat GLExampleApplication::RegularizeDistance(GLfloat d) const
{
  if(fabsf(d) < minDistance) return 0.0f;
  return d;
}
