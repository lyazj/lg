#include <string.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GLApplication.h"
#include "GLBarnsleyFern.h"
#include "GLCircle.h"
#include "GLColorDecorator.h"
#include "GLKochSnowflake.h"
#include "GLMaze.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLSierpinskiGasket.h"
#include "GLSierpinskiGasketChaos.h"
#include "GLSmiley.h"
#include "GLTriangle.h"
#include "Utils.h"

using namespace std;

class GLExampleApplication final : public GLApplication {
public:
  using GLApplication::GLApplication;

  void PreInit() override;
  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;

  void InitRenderable();
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
  SetFrameRate(0);
  SetShowFrameRateInterval(0);
}

void GLExampleApplication::Init()
{
  GLApplication::Init();
  UseProgram(GLProgram::GetDefaultProgram());

  InitRenderable();
  renderable->SetVertexAttributes();
  renderable->Buffer();

  Display();
  SaveScreen(GetScreenshotName(), GL_FRONT);
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::InitRenderable()
{
  if(argc == 1) {
    cerr << "Usage: " << ToLocaleString(GetProgramShortName()) << " <renderable> [ <args> ... ]" << endl;
    exit(EXIT_FAILURE);
  }

  if(strcmp(argv[1], "GLTriangle") == 0) {
    renderable = make_shared<GLTriangle>(vec3(0.0f, 0.5f, 0.0f), vec3(-0.5f, -0.5f, 0.0f), vec3(0.5f, -0.5f, 0.0f));
    return;
  }

  if(strcmp(argv[1], "GLRectangle") == 0) {
    renderable = make_shared<GLRectangle>(1.0f, 0.8f);
    return;
  }

  if(strcmp(argv[1], "GLCircle") == 0) {
    GLint segments = 64;
    if(argc > 2) segments = stoi(argv[2]);
    renderable = make_shared<GLCircle>(0.8f, segments);
    return;
  }

  if(strcmp(argv[1], "GLSmiley") == 0) {
    GLint segments = 64;
    if(argc > 2) segments = stoi(argv[2]);
    renderable = make_shared<GLSmiley>(0.8f, segments);
    return;
  }

  if(strcmp(argv[1], "GLSierpinskiGasket") == 0) {
    GLint order = 8;
    GLfloat noise = 0.0f;
    if(argc > 2) order = stoi(argv[2]);
    if(argc > 3) noise = stof(argv[3]);
    renderable = make_shared<GLSierpinskiGasket>(order, noise);
    SetModel(translate(GetModel(), vec3(0.0f, -0.25f, 0.0f)));
    return;
  }

  if(strcmp(argv[1], "GLSierpinskiGasketChaos") == 0) {
    GLint points = 1e6;
    if(argc > 2) points = stoi(argv[2]);
    renderable = make_shared<GLSierpinskiGasketChaos>(points);
    SetModel(translate(GetModel(), vec3(0.0f, -0.25f, 0.0f)));
    return;
  }

  if(strcmp(argv[1], "GLKochSnowflake") == 0) {
    GLint order = 8;
    if(argc > 2) order = stoi(argv[2]);
    renderable = make_shared<GLKochSnowflake>(order);
    return;
  }

  if(strcmp(argv[1], "GLBarnsleyFern") == 0) {
    GLint points = 1e6;
    if(argc > 2) points = stoi(argv[2]);
    auto fern = make_shared<GLBarnsleyFern>(points);
    fern->Normalize();
    renderable = make_shared<GLUniformColorDecorator>(fern, vec4(0.0f, 1.0f, 0.0f, 1.0f));
    return;
  }

  if(strcmp(argv[1], "GLMaze") == 0) {
    GLint w = 20, h = 20;
    GLMazeType type = GLMazeType::Backtracking;
    if(argc > 2) w = stoi(argv[2]);
    if(argc > 3) h = stoi(argv[3]);
    if(argc > 4) {
      if(strcmp(argv[4], "Backtracking") == 0) {
        type = GLMazeType::Backtracking;
      } else if(strcmp(argv[4], "Prim") == 0) {
        type = GLMazeType::Prim;
      } else if(strcmp(argv[4], "Kruskal") == 0) {
        type = GLMazeType::Kruskal;
      } else {
        cerr << "Unknown maze type: " << argv[4] << endl;
        exit(EXIT_FAILURE);
      }
    }
    renderable = make_shared<GLMaze>(w, h, type);
    return;
  }

  cerr << "Unknown renderable: " << argv[1] << endl;
  exit(EXIT_FAILURE);
}

string GLExampleApplication::GetScreenshotName() const
{
  string name = argv[1];
  for(int i = 2; i < argc; i++) (name += "_") += argv[i];
  name += ".png";
  return name;
}
