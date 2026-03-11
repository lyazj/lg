#include "GLColorableGeometry.h"

#include "GLBufferedGeometry.h"
#include "GLProgram.h"

void GLColorableGeometry::Buffer() const
{
  renderable->Buffer();
  Color();
  colorBuffer.Buffer(colors);
}

void GLColorableGeometry::Draw(const mat4 &model) const
{
  renderable->GetVertexArray().Bind();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
  renderable->Draw(model);
  GLProgram::DisableVertexAttribute("a_color");
}

void GLFlatColorGeometry::Color() const { colors.assign(renderable->GetNVertex(), color); }

void GLRandomColorGeometry::Color() const
{
  GLint n = renderable->GetNVertex();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) colors.push_back(vec4(drand48(), drand48(), drand48(), 1.0));
}
