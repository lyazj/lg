#include "GLColorDecorator.h"

#include "GLProgram.h"
#include "GLVertexArray.h"

void GLColorDecorator::Buffer() const
{
  renderable->Buffer();
  Color();
  colorBuffer.Buffer(colors);
}

void GLColorDecorator::Draw(const mat4 &model) const
{
  renderable->GetVertexArray().Bind();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
  renderable->Draw(model);
  GLProgram::DisableVertexAttribute("a_color");
}

void GLFlatColorDecorator::Color() const { colors.assign(renderable->GetNVertex(), color); }

void GLRandomColorDecorator::Color() const
{
  GLint n = renderable->GetNVertex();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) colors.push_back(vec4(drand48(), drand48(), drand48(), 1.0));
}
