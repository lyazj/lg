#include "GLColorDecorator.h"

#include <glm/vec3.hpp>

#include "GLProgram.h"
#include "GLVertexArray.h"
#include "Utils.h"

using namespace std;

void GLUniformColorDecorator::IssueDraw(const mat4 &model) const
{
  GLProgram::SetVertexAttribute("a_color", color);
  GLColorDecorator::IssueDraw(model);
  GLProgram::SetDefaultVertexAttribute("a_color");
}

void GLBufferedColorDecorator::Buffer() const
{
  GLColorDecorator::Buffer();
  SetColors();
  colorBuffer.Buffer(colors);
}

void GLBufferedColorDecorator::IssueDraw(const mat4 &model) const
{
  GetVertexArray().Bind();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
  GLColorDecorator::IssueDraw(model);
  GLProgram::DisableVertexAttribute("a_color");
}

void GLRandomColorDecorator::SetColors() const
{
  GLint n = GetNVertex();
  colors.clear();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) colors.push_back(vec4(RandVec3(), 1.0f));
}
