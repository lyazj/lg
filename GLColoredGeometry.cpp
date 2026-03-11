#include "GLColoredGeometry.h"

#include "GLElementGeometry.h"
#include "GLProgram.h"

void GLColoredGeometry::Buffer() const
{
  geometry->Buffer();
  Color();
  colorBuffer.Buffer(colors);
}

void GLColoredGeometry::Draw(const mat4 &model) const
{
  geometry->GetVertexArray().Bind();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
  geometry->Draw(model);
  GLProgram::DisableVertexAttribute("a_color");
}

void GLSingleColorGeometry::Color() const { colors.assign(geometry->GetNVertex(), color); }

void GLRandomColorGeometry::Color() const
{
  GLint n = geometry->GetNVertex();
  colors.reserve(n);
  for(GLint i = 0; i < n; ++i) colors.push_back(vec4(drand48(), drand48(), drand48(), 1.0));
}
