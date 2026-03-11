#include <string.h>

#include <glm/gtx/transform.hpp>
#include <iostream>

#include "BarnsleyFern.h"
#include "GLApplication.h"
#include "GLCircle.h"
#include "GLColoredGeometry.h"
#include "GLProgram.h"
#include "GLSmell.h"
#include "GLTriangle.h"
#include "KochSnowflake.h"
#include "Maze.h"
#include "SierpinskiGasket.h"
#include "SierpinskiGasket2.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLGeometryPtr geometry;

  void InitGeometry();
  string GetScreenshotName() const;
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

  SetTitle("GLApplication Example");
}

void GLExampleApplication::Init()
{
  GLApplication::Init();
  UseProgram(GLProgram::GetDefaultProgram());

  InitGeometry();
  geometry->SetVertexAttributes();
  geometry->Buffer();

  Display();
  SaveScreen(GetScreenshotName());
}

void GLExampleApplication::Display()
{
  Clear();
  geometry->Draw(model);
  Flush();
}

void GLExampleApplication::InitGeometry()
{
  if(argc == 1) {
    cerr << "Usage: " << GetProgramShortName() << " <geometry> [ <args> ... ]" << endl;
    exit(EXIT_FAILURE);
  }

  if(strcmp(argv[1], "GLTriangle") == 0) {
    geometry.reset(new GLTriangle({ 0.0f, 0.5f, 0.0f }, { -0.5f, -0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f }));
    return;
  }

  if(strcmp(argv[1], "GLCircle") == 0) {
    GLint segments = 64;
    if(argc > 2) segments = stoi(argv[2]);
    geometry = make_shared<GLCircle>(0.8f, segments);
    return;
  }

  if(strcmp(argv[1], "GLSmell") == 0) {
    GLint segments = 64;
    if(argc > 2) segments = stoi(argv[2]);
    geometry = make_shared<GLSmell>(0.8f, segments);
    return;
  }

  if(strcmp(argv[1], "SierpinskiGasket") == 0) {
    GLint order = 8;
    GLfloat noise = 0.0f;
    if(argc > 2) order = stoi(argv[2]);
    if(argc > 3) noise = stof(argv[3]);
    geometry = make_shared<SierpinskiGasket>(order, noise);
    SetModel(glm::translate(model, vec3(0.0f, -0.25f, 0.0f)));
    return;
  }

  if(strcmp(argv[1], "SierpinskiGasket2") == 0) {
    GLint points = 1e6;
    if(argc > 2) points = stoi(argv[2]);
    geometry = make_shared<SierpinskiGasket2>(points);
    SetModel(glm::translate(model, vec3(0.0f, -0.25f, 0.0f)));
    return;
  }

  if(strcmp(argv[1], "KochSnowflake") == 0) {
    GLint order = 8;
    if(argc > 2) order = stoi(argv[2]);
    geometry = make_shared<KochSnowflake>(order);
    return;
  }

  if(strcmp(argv[1], "BarnsleyFern") == 0) {
    GLint points = 1e6;
    if(argc > 2) points = stoi(argv[2]);
    auto fern = make_shared<BarnsleyFern>(points);
    fern->Normalize();
    geometry.reset(new GLSingleColorGeometry(fern, { 0.0, 1.0, 0.0, 1.0 }));
    return;
  }

  if(strcmp(argv[1], "Maze") == 0) {
    GLint w = 20, h = 20;
    MazeType type = MazeType::Backtracking;
    if(argc > 2) w = stoi(argv[2]);
    if(argc > 3) h = stoi(argv[3]);
    if(argc > 4) {
      if(strcmp(argv[4], "Backtracking") == 0) {
        type = MazeType::Backtracking;
      } else if(strcmp(argv[4], "Prim") == 0) {
        type = MazeType::Prim;
      } else if(strcmp(argv[4], "Kruskal") == 0) {
        type = MazeType::Kruskal;
      } else {
        cerr << "Unknown maze type: " << argv[4] << endl;
        exit(EXIT_FAILURE);
      }
    }
    geometry = make_shared<Maze>(w, h, type);
    return;
  }

  cerr << "Unknown geometry: " << argv[1] << endl;
  exit(EXIT_FAILURE);
}

string GLExampleApplication::GetScreenshotName() const
{
  string name = argv[1];
  for(int i = 2; i < argc; i++) (name += "_") += argv[i];
  name += ".png";
  return name;
}
