#include <glm/gtx/transform.hpp>
#include <iostream>

#include "GLApplication.h"
#include "GLCompositeGeometry.h"
#include "GLProgram.h"
#include "GLSmell.h"
#include "GLTransformedGeometry.h"
#include "GLTriangle.h"
#include "Maze.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLint width, height;
  shared_ptr<Maze> maze;
  shared_ptr<GLTransformedGeometry> rat;
  GLGeometryPtr geometry;

  GLint ratDirection;
  GLint ratX, ratY, exitX, exitY;

  void UpdateRatGeometry();
  void Mouse(int button, int state, int x, int y);
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

  SetTitle("Maze Example");
  EnableDoubleBuffer();
}

void GLExampleApplication::Init()
{
  GLApplication::Init();
  glutMouseFunc(
      [](int b, int s, int x, int y) { ((GLExampleApplication *)GLApplication::GetInstance())->Mouse(b, s, x, y); });

  UseProgram(GLProgram::GetDefaultProgram());

  if(argc != 3) {
    cerr << "Usage: " << GetProgramShortName() << " <width> <height>" << endl;
    exit(EXIT_FAILURE);
  }
  width = stoi(argv[1]);
  height = stoi(argv[2]);

  maze = make_shared<Maze>(width, height, MazeType::Kruskal);
  GLfloat ratRadius = min(0.8f / (GLfloat)width, 0.8f / (GLfloat)height);
  vec3 ratA = { ratRadius, 0.0f, 0.0f };
  vec3 ratB = { -ratRadius, ratRadius * 0.5f, 0.0f };
  vec3 ratC = { -ratRadius, -ratRadius * 0.5f, 0.0f };
  rat = make_shared<GLTransformedGeometry>(make_shared<GLTriangle>(ratA, ratB, ratC), mat4(1.0));
  ratDirection = 0;
  maze->GetEntry(ratX, ratY);
  maze->GetExit(exitX, exitY);
  UpdateRatGeometry();

  auto scene = make_shared<GLCompositeGeometry>();
  scene->AddGeometry(maze);
  scene->AddGeometry(rat);
  geometry = scene;

  geometry->SetVertexAttributes();
  geometry->Buffer();
}

void GLExampleApplication::Display()
{
  Clear();
  geometry->Draw(model);
  Flush();
}

void GLExampleApplication::UpdateRatGeometry()
{
  GLfloat x, y;
  maze->GetNormalizedPosition((GLfloat)ratX + 0.5f, (GLfloat)ratY + 0.5f, x, y);
  mat4 m = glm::translate(vec3(x, y, 0.0));
  m = glm::rotate(m, (GLfloat)ratDirection * 90.0f * deg, vec3(0.0f, 0.0f, 1.0f));
  rat->SetModel(m);
}

void GLExampleApplication::Mouse(int button, int state, int, int)
{
  if(state == GLUT_UP) return;
  if(state != GLUT_DOWN) abort();
  switch(button) {  // My mouse also generates 3 (scroll-up), 4 (scroll-down), 7 (backward), and 8 (forward).
  case GLUT_LEFT_BUTTON: TurnLeft(); break;
  case GLUT_RIGHT_BUTTON: TurnRight(); break;
  case GLUT_MIDDLE_BUTTON: MoveForward(); break;
  default: cerr << "Warning: unknown mouse button " << button << endl; break;  // So don't abort() here.
  }
  UpdateRatGeometry();
  CheckWin();
  glutPostRedisplay();
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
  glutMouseFunc(nullptr);
  geometry.reset(new GLSmell(0.8f, 256));
  geometry->SetVertexAttributes();
  geometry->Buffer();
}
