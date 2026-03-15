#include "GLSimpleRenderableDecorator.h"

#include "GLApplication.h"

using namespace std;

void GLSimpleRenderableDecorator::Draw(const mat4 &model) const
{
  if(!program) return renderable->Draw(model);
  GLApplication *app = GLApplication::GetInstance();
  GLProgramPtr previousProgram = app->GetProgram();
  app->UseProgram(program);
  renderable->Draw(model);
  app->UseProgram(previousProgram);
}
