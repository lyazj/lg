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

void GLBufferedGeometry::IssueSetVertexAttributes(bool) const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", GetDimension());
}

void GLBufferedGeometry::IssueBuffer(bool) const
{
  vertexArray.Bind();
  IssueBuffer();
}

void GLBufferedGeometry::IssueDraw(const mat4 &model) const
{
  vertexArray.Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLBufferedGeometry::SetUniforms(const mat4 &model) const
{
  // Current program MUST NOT be null.
  GLApplication::GetInstance()->GetProgram()->SetUniform("u_model", model);
}
