#include <glm/gtx/transform.hpp>
#include <iostream>

#include "GLApplication.h"
#include "GLCompositeGeometry.h"
#include "GLProgram.h"
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
  UseProgram(GLProgram::GetDefaultProgram());

  if(argc != 3) {
    cerr << "Usage: " << GetProgramShortName() << " <width> <height>" << endl;
    exit(EXIT_FAILURE);
  }
  width = stoi(argv[1]);
  height = stoi(argv[2]);

  maze = make_shared<Maze>(width, height, MazeType::Kruskal);
  GLfloat ratRadius = min(0.8f / (GLfloat)width, 0.8f / (GLfloat)height);
  glm::vec3 ratA = { ratRadius, 0.0f, 0.0f };
  glm::vec3 ratB = { -ratRadius, ratRadius * 0.5f, 0.0f };
  glm::vec3 ratC = { -ratRadius, -ratRadius * 0.5f, 0.0f };
  rat = make_shared<GLTransformedGeometry>(make_shared<GLTriangle>(ratA, ratB, ratC), glm::mat4(1.0));
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
  glm::mat4 m = glm::translate(glm::vec3(x, y, 0.0));
  m = glm::rotate(m, (GLfloat)ratDirection * 90.0f * deg, glm::vec3(0.0f, 0.0f, 1.0f));
  rat->SetModel(m);
}
