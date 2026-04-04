#include "GLBufferedGeometry.h"

#include "GLApplication.h"
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

void GLBufferedGeometry::Draw(const mat4 &model) const
{
  Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLBufferedGeometry::IssueSetVertexAttributes(bool) const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", GetDimension());
}

void GLBufferedGeometry::IssueBuffer(bool) const
{
  Bind();
  IssueBuffer();
}

void GLBufferedGeometry::Bind() const { vertexArray.Bind(); }

void GLBufferedGeometry::SetUniforms(const mat4 &model) const
{
  // Current program MUST NOT be null.
  GLApplication::GetInstance()->GetProgram()->SetUniform("u_model", model);
}
