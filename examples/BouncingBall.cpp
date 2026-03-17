#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
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
  void SetColors() const override { }
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
  ~GLExampleApplication() override;

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
  unsigned recordDuration;
  unsigned ts = 0;  // safe when wraps around
  vector<unsigned> tss;
  bool recordCreated = false;
  thread recordThread;

  void Frame(uint64_t t, uint64_t dt) override;
  void UpdateBallTransform();
  void CreateRecord();
};

int main(int argc, char *argv[])
{
  static GLApplicationPtr application;
  application = make_shared<GLExampleApplication>(argc, argv);
  application->Run();
  return 0;
}

GLExampleApplication::~GLExampleApplication()
{
  if(recordThread.joinable()) recordThread.join();
}

void GLExampleApplication::PreInit()
{
  GL3DApplication::PreInit();
  EnableDoubleBuffer();
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();

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
  auto r = make_shared<GLTransformedRenderable>(rect, glm::rotate(mat4(1.0f), -90.0f * deg, vec3(1.0f, 0.0f, 0.0f)));
  scene->AddGeometry(r);

  // The ball.
  auto sphere = make_shared<GLExampleSphere>(ballRadius, 64, 32);
  ball = make_shared<GLTransformedRenderable>(sphere, glm::translate(mat4(1.0f), vec3(0.0f, ballInitialHeight, 0.0f)));
  scene->AddGeometry(ball);

  scene->SetVertexAttributes();
  scene->Buffer();

  period = sqrtf(2 * ballInitialHeight / g) * 2.0f;    // s
  time = 0.0f;                                         // s
  recordDuration = unsigned(3000.0f * period + 0.5f);  // ms
  ballHeight = ballInitialHeight;                      // m
}

void GLExampleApplication::Display()
{
  Clear();
  scene->Draw(GetModel());
  if(!recordCreated && ts < recordDuration) {
    SaveScreen("BouncingBall_" + to_string(ts) + ".png", GL_BACK);
    tss.push_back(ts);
  }
  Flush();
  if(!recordCreated && ts >= recordDuration) {
    recordCreated = true;
    recordThread = thread(&GLExampleApplication::CreateRecord, this);
  }
}

void GLExampleApplication::Frame(uint64_t t, uint64_t dt)
{
  ts = (unsigned)NsToMs(t);
  time += (GLfloat)dt * 1e-9f;  // s
  if(Expect(time >= period, false)) {
    time -= period;
    // Handle cases where Idle() is delayed (e.g., after a long pause).
    if(Expect(time >= period, false)) time -= floorf(time / period) * period;
  }

  GLfloat t0 = time;
  if(t0 > period / 2.0f) t0 = period - t0;
  ballHeight = ballInitialHeight - 0.5f * g * t0 * t0;  // m

  UpdateBallTransform();
}

void GLExampleApplication::UpdateBallTransform()
{
  ball->SetModel(glm::translate(mat4(1.0f), vec3(0.0f, ballHeight, 0.0f)));
}

void GLExampleApplication::CreateRecord()  // Called in a separate thread.
{
  ofstream ofs("BouncingBall.txt");
  tss.push_back(recordDuration);
  for(size_t i = 0; i + 1 < tss.size(); ++i) {
    ofs << "file BouncingBall_" << tss[i] << ".png\n";
    ofs << "duration " << GLfloat(tss[i + 1] - tss[i]) * 0.001f << "\n";
  }
  ofs.close();

  const char *cmd =  // Command line suggested by ChatGPT. Controls quality and file size.
      "ffmpeg -y -f concat -safe 0 -i BouncingBall.txt -c:v libx264 -crf 12 -preset slow -pix_fmt yuv420p "
      "-movflags +faststart BouncingBall.mp4";
  if(system(cmd)) { }  // We can do nothing on error.

  remove("BouncingBall.txt");
  for(unsigned t : tss) remove(("BouncingBall_" + to_string(t) + ".png").c_str());
  tss.clear();  // tss is never accessed in the main thread after this point.
}
