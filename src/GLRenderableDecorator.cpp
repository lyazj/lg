#include "GLRenderableDecorator.h"

#include "GLApplication.h"

using namespace std;

void GLRenderableDecorator::IssueDraw(const mat4 &model) const
{
  if(!program) return DecoratorDraw(model);
  GLProgramGuard guard(program);
  DecoratorDraw(model);
}
