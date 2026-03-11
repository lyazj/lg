#include "GLElementGeometry.h"

#include "GLBuffer.h"
#include "GLProgram.h"

using namespace std;

GLElementGeometry::GLElementGeometry()
{
  // empty
}

GLElementGeometry::~GLElementGeometry()
{
  // empty
}

void GLElementGeometry::SetVertexAttributes() const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", GetDimension());
}

void GLElementGeometry::Buffer() const
{
  Bind();
  IssueBuffer();
}

void GLElementGeometry::Draw(const mat4 &model) const
{
  Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLElementGeometry::Bind() const { vertexArray.Bind(); }

void GLElementGeometry::SetUniforms(const mat4 &model) const
{
  GLProgram::GetCurrentProgram()->SetUniform("u_model", model);
}
