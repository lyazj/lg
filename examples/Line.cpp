#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GLApplication.h"
#include "GLBuffer.h"
#include "GLPoints.h"
#include "GLProgram.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLPoints)

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;
  void MouseDown(MouseButton button, int x, int y) override;

private:
  GLRenderablePtr renderable;

  void InitModel();
  void UpdateRenderable();
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
#ifdef _WIN32
  glPointSize(1.0f);
#else  /* _WIN32 */
  glPointSize(2.0f);
#endif /* _WIN32 */
  UseProgram(GLProgram::GetDefaultProgram());

  InitModel();
  UpdateRenderable();
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::MouseDown(MouseButton button, int, int)
{
  if(button == MouseButton::LeftButton) {
    UpdateRenderable();
    PostRedisplay();
  }
}

void GLExampleApplication::InitModel()
{
  mat4 m(1.0f);
  m = glm::translate(m, vec3(-1.0f, -1.0f, 0.0f));
  m = glm::scale(m, vec3(2.0f / (GLfloat)GetWindowWidth(), 2.0f / (GLfloat)GetWindowHeight(), 1.0f));
  SetModel(m);
}

static void Bresenham(const GLPointsPtr &points, GLint x1, GLint y1, GLint x2, GLint y2);

void GLExampleApplication::UpdateRenderable()
{
  auto points = make_shared<GLPoints>();
  points->GetVertexBuffer().SetUsage(GL_DYNAMIC_DRAW);
  GLint w = GetWindowWidth(), h = GetWindowHeight();

  vec2 p1 = 2.0f * RandVec2() - 1.0f;
  vec2 p2 = 2.0f * RandVec2() - 1.0f;
  cout << "p1: " << p1 << ", p2: " << p2 << endl;
  GLint x1 = GLint((p1.x + 1.0f) * (GLfloat)w * 0.5f - 0.5f);  // [0, w - 1]
  GLint y1 = GLint((p1.y + 1.0f) * (GLfloat)h * 0.5f - 0.5f);  // [0, h - 1]
  GLint x2 = GLint((p2.x + 1.0f) * (GLfloat)w * 0.5f - 0.5f);  // [0, w - 1]
  GLint y2 = GLint((p2.y + 1.0f) * (GLfloat)h * 0.5f - 0.5f);  // [0, h - 1]
  cout << "x1: " << x1 << ", y1: " << y1 << ", x2: " << x2 << ", y2: " << y2 << endl;
  Bresenham(points, x1, y1, x2, y2);

  renderable = points;
  renderable->SetVertexAttributes(false);
  renderable->Buffer(false);
}

static void Bresenham(const GLPointsPtr &points, GLint x1, GLint y1, GLint x2, GLint y2, bool yFlip, bool xySwap);

void Bresenham(const GLPointsPtr &points, GLint x1, GLint y1, GLint x2, GLint y2)
{
  bool yFlip = false, xySwap = false;
  if(x2 - x1 < 0) {
    swap(x1, x2);
    swap(y1, y2);
  }  // x1 <= x2
  if(y2 - y1 < 0) {
    yFlip = true;
    y1 = -y1;
    y2 = -y2;
  }  // y1 <= y2
  if(x2 - x1 < y2 - y1) {
    xySwap = true;
    swap(x1, y1);
    swap(x2, y2);
  }  // dx >= dy
  Bresenham(points, x1, y1, x2, y2, yFlip, xySwap);
}

static void AddPoint(const GLPointsPtr &points, GLfloat x, GLfloat y, bool yFlip, bool xySwap);

void Bresenham(const GLPointsPtr &points, GLint x1, GLint y1, GLint x2, GLint y2, bool yFlip, bool xySwap)
{
  x2 += (x1 == x2);  // Handle the degenerated case.
  GLint dx = x2 - x1, dy = y2 - y1, d = dx - dy, y = y1;
  for(GLint x = x1; x < x2; ++x) {  // d is for the next point
    if(d >= 0) {
      d -= 2 * dy;
    } else {
      ++y, d -= 2 * (dy - dx);
    }
    AddPoint(points, (GLfloat)x + 0.5f, (GLfloat)y + 0.5f, yFlip, xySwap);
  }
}

void AddPoint(const GLPointsPtr &points, GLfloat x, GLfloat y, bool yFlip, bool xySwap)
{
  if(xySwap) swap(x, y);
  if(yFlip) y = -y;
  points->AddVertex(vec3(x, y, 0.0f));
}
