#include <glm/gtx/transform.hpp>
#include <vector>

#include "GL3DApplication.h"
#include "GLAxes.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLSphere.h"
#include "GLTransformedRenderable.h"
#include "Utils.h"

using namespace std;

class GLExampleSphere : public GLBufferedColorDecorator {
public:
  GLExampleSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLExampleSphere() override = default;

protected:
  void SetColor() const override { }
};

GLExampleSphere::GLExampleSphere(GLfloat r, GLint sl, GLint st)
    : GLBufferedColorDecorator(make_shared<GLSphere>(r, sl, st))
{
  GLint n = renderable->GetNVertex();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) {
    vec3 vertex;
    renderable->GetVertex(i, vertex);
    GLfloat lambda = vertex.z / r / 2.0f + 0.5f;
    colors.emplace_back(lambda, lambda, lambda, 1.0f);
  }
}

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  inline static constexpr GLfloat g = 9.80665f;  // m * s^-2

  GLfloat ballRadius = 0.5f;         // m
  GLfloat ballInitialHeight = 5.0f;  // m
  GLTransformedRenderablePtr ball;
  GLCompositeRenderablePtr scene;

  GLfloat period, time, ballHeight;

  void Idle();
  void UpdateBallTransform();
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

  SetTitle("Bouncing Ball Example");
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  glutIdleFunc([] { ((GLExampleApplication *)GLApplication::GetInstance())->Idle(); });

  vec3 camera(10.0f, 10.0f, 10.0f);  // m
  vec3 target(0.0f, 0.0f, 0.0f);
  vec3 up(0.0f, 1.0f, 0.0f);
  SetView(glm::lookAt(camera, target, up));
  SetProjection(45.0f * deg, 0.1f, 100.0f);  // m

  UseProgram(GLProgram::GetDefaultProgram());

  scene = make_shared<GLCompositeRenderable>();

  // Axes.
  scene->AddGeometry(make_shared<GLAxes>(10.0f, 10.0f, 10.0f));  // m

  // The y = 0 Plane.
  GLSimpleRenderablePtr rect = make_shared<GLRectangle>(10.0f, 10.0f);  // m
  rect = make_shared<GLUniformColorDecorator>(rect, vec4(0.95f, 0.95f, 0.95f, 1.0f));
  auto rectangle = make_shared<GLTransformedRenderable>(rect, glm::rotate(-90.0f * deg, vec3(1.0f, 0.0f, 0.0f)));
  scene->AddGeometry(rectangle);

  // The ball.
  auto sphere = make_shared<GLExampleSphere>(ballRadius, 64, 32);
  ball = make_shared<GLTransformedRenderable>(sphere, glm::translate(vec3(0.0f, ballInitialHeight, 0.0f)));
  scene->AddGeometry(ball);

  scene->SetVertexAttributes();
  scene->Buffer();

  period = sqrtf(2 * ballInitialHeight / g) * 2.0f;  // s
  time = 0.0f;                                       // s
  ballHeight = ballInitialHeight;                    // m
}

void GLExampleApplication::Display()
{
  Clear();
  scene->Draw(model);
  Flush();
}

void GLExampleApplication::Idle()
{
  static int ts;  // int: safe when overflows
  int t = GetElapsedTime();
  if((t - ts) * 60ULL < 1000ULL) return;
  GLfloat dt = GLfloat(t - ts) / 1000.0f;  // s
  ts = t;

  time += dt;  // s
  if(__builtin_expect(time >= period, false)) {
    time -= period;
    // Handle cases where Idle() is delayed (e.g., after a long pause).
    if(__builtin_expect(time >= period, false)) time -= floorf(time / period) * period;
  }

  GLfloat t0 = time;
  if(t0 > period / 2.0f) t0 = period - t0;
  ballHeight = ballInitialHeight - 0.5f * g * t0 * t0;  // m

  UpdateBallTransform();
  glutPostRedisplay();
}

void GLExampleApplication::UpdateBallTransform() { ball->SetModel(glm::translate(vec3(0.0f, ballHeight, 0.0f))); }
