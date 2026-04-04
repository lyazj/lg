#include "GLRenderableDecorator.h"

#include "GLApplication.h"

using namespace std;

void GLRenderableDecorator::Draw(const mat4 &model) const
{
  if(!program) return IssueDraw(model);
  GLProgramGuard guard(program);
  IssueDraw(model);
}
