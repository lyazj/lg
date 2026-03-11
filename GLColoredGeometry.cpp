#include "GLColoredGeometry.h"

#include "GLBasicGeometry.h"
#include "GLProgram.h"

void GLColoredGeometry::SetVertexAttributes() const { geometry->SetVertexAttributes(); }

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
