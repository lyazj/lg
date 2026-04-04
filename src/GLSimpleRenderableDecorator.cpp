#include "GLSimpleRenderableDecorator.h"

#include "GLApplication.h"

using namespace std;

void GLSimpleRenderableDecorator::IssueDraw(const mat4 &model) const
{
  if(!program) return DecoratorDraw(model);
  GLProgramGuard guard(program);
  DecoratorDraw(model);
}
