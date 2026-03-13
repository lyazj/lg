#include "GLColorDecorator.h"

#include <glm/vec3.hpp>

#include "GLProgram.h"
#include "GLVertexArray.h"
#include "Utils.h"

void GLUniformColorDecorator::Draw(const mat4 &model) const
{
  GLProgram::SetVertexAttribute("a_color", color);
  renderable->Draw(model);
  GLProgram::SetDefaultVertexAttribute("a_color");
}

void GLBufferedColorDecorator::Buffer() const
{
  renderable->Buffer();
  SetColor();
  colorBuffer.Buffer(colors);
}

void GLBufferedColorDecorator::Draw(const mat4 &model) const
{
  renderable->GetVertexArray().Bind();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
  renderable->Draw(model);
  GLProgram::DisableVertexAttribute("a_color");
}

void GLRandomColorDecorator::SetColor() const
{
  GLint n = renderable->GetNVertex();
  colors.clear();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) colors.push_back(vec4(RandVec3(), 1.0));
}
