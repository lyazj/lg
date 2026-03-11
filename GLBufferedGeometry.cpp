#include "GLBufferedGeometry.h"

#include "GLBuffer.h"
#include "GLProgram.h"

using namespace std;

GLBufferedGeometry::GLBufferedGeometry()
{
  // empty
}

GLBufferedGeometry::~GLBufferedGeometry()
{
  // empty
}

void GLBufferedGeometry::SetVertexAttributes() const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", GetDimension());
}

void GLBufferedGeometry::Buffer() const
{
  Bind();
  IssueBuffer();
}

void GLBufferedGeometry::Draw(const mat4 &model) const
{
  Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLBufferedGeometry::Bind() const { vertexArray.Bind(); }

void GLBufferedGeometry::SetUniforms(const mat4 &model) const
{
  GLProgram::GetCurrentProgram()->SetUniform("u_model", model);
}
