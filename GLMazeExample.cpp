#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GLApplication.h"
#include "GLColorDecorator.h"
#include "GLCompositeRenderable.h"
#include "GLMaze.h"
#include "GLProgram.h"
#include "GLSmiley.h"
#include "GLTransformedRenderable.h"
#include "GLTriangle.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLint width, height;
  shared_ptr<GLMaze> maze;
  GLTransformedRenderablePtr rat;
  GLRenderablePtr renderable;

  GLint ratDirection;
  GLint ratX, ratY, exitX, exitY;
  bool won;

  void UpdateRatGeometry();
  void MouseDown(Mouse button, int x, int y) override;
  void KeyDown(unsigned char key, int x, int y) override;
  void SpecialKeyDown(SpecialKey key, int x, int y) override;
  void TurnLeft() { ratDirection = (ratDirection + 1) % 4; }
  void TurnRight() { ratDirection = (ratDirection + 3) % 4; }
  void MoveForward();
  bool GetForwardPosition(GLint &x, GLint &y);
  void CheckWin();
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

  SetTitle("GLMaze Example");
  EnableDoubleBuffer();
}

void GLExampleApplication::Init()
{
  GLApplication::Init();

  UseProgram(GLProgram::GetDefaultProgram());

  if(argc != 3) {
    cerr << "Usage: " << programShortName << " <width> <height>" << endl;
    exit(EXIT_FAILURE);
  }
  width = stoi(argv[1]);
  height = stoi(argv[2]);

  maze = make_shared<GLMaze>(width, height, GLMazeType::Kruskal);
  GLfloat ratRadius = min(0.8f / (GLfloat)width, 0.8f / (GLfloat)height);
  vec3 ratA = { ratRadius, 0.0f, 0.0f };
  vec3 ratB = { -ratRadius, ratRadius * 0.5f, 0.0f };
  vec3 ratC = { -ratRadius, -ratRadius * 0.5f, 0.0f };
  GLSimpleRenderablePtr rawRat = make_shared<GLTriangle>(ratA, ratB, ratC);
  rawRat = make_shared<GLUniformColorDecorator>(rawRat, vec4(0.0f, 1.0f, 0.0f, 1.0f));
  rat = make_shared<GLTransformedRenderable>(rawRat, mat4(1.0f));
  ratDirection = 0;
  maze->GetEntry(ratX, ratY);
  maze->GetExit(exitX, exitY);
  UpdateRatGeometry();
  won = false;

  auto scene = make_shared<GLCompositeRenderable>();
  scene->AddGeometry(maze);
  scene->AddGeometry(rat);
  renderable = scene;

  renderable->SetVertexAttributes();
  renderable->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(model);
  Flush();
}

void GLExampleApplication::UpdateRatGeometry()
{
  GLfloat x, y;
  maze->GetNormalizedPosition((GLfloat)ratX + 0.5f, (GLfloat)ratY + 0.5f, x, y);
  mat4 m = glm::translate(mat4(1.0f), vec3(x, y, 0.0f));
  m = glm::rotate(m, (GLfloat)ratDirection * 90.0f * deg, vec3(0.0f, 0.0f, 1.0f));
  rat->SetModel(m);
}

void GLExampleApplication::MouseDown(Mouse button, int, int)
{
  if(won) exit(EXIT_SUCCESS);
  switch(button) {
  case Mouse::LeftButton: TurnLeft(); break;
  case Mouse::RightButton: TurnRight(); break;
  case Mouse::MiddleButton: MoveForward(); break;
  default: break;
  }
  UpdateRatGeometry();
  CheckWin();
  PostRedisplay();
}

void GLExampleApplication::KeyDown(unsigned char key, int x, int y)
{
  GLApplication::KeyDown(key, x, y);
  switch(key) {
  case 'L':
  case 'l': return MouseDown(Mouse::LeftButton, x, y);
  case ' ': return MouseDown(Mouse::MiddleButton, x, y);
  case 'R':
  case 'r': return MouseDown(Mouse::RightButton, x, y);
  }
}

void GLExampleApplication::SpecialKeyDown(SpecialKey key, int x, int y)
{
  switch(key) {
  case SpecialKey::Right: return ratDirection = 0, KeyDown(' ', x, y);
  case SpecialKey::Up: return ratDirection = 1, KeyDown(' ', x, y);
  case SpecialKey::Left: return ratDirection = 2, KeyDown(' ', x, y);
  case SpecialKey::Down: return ratDirection = 3, KeyDown(' ', x, y);
  default: break;
  }
}

void GLExampleApplication::MoveForward()
{
  GLint x, y;
  if(!GetForwardPosition(x, y)) return;
  ratX = x, ratY = y;
}

bool GLExampleApplication::GetForwardPosition(GLint &x, GLint &y)
{
  auto cell = maze->GetCells()[ratX][ratY];
  switch(ratDirection) {
  case 0:
    if(!cell.right) return false;
    x = ratX, y = ratY + 1;
    return true;
  case 1:
    if(!cell.up) return false;
    x = ratX - 1, y = ratY;
    return true;
  case 2:
    if(!cell.left) return false;
    x = ratX, y = ratY - 1;
    return true;
  case 3:
    if(!cell.down) return false;
    x = ratX + 1, y = ratY;
    return true;
  default: abort();  // never reached
  }
}

void GLExampleApplication::CheckWin()
{
  if(ratX != exitX || ratY != exitY) return;
  won = true;
  renderable.reset(new GLSmiley(0.8f, 256));
  renderable->SetVertexAttributes();
  renderable->Buffer();
}
