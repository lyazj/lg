#include "GLBasicGeometry.h"

#include <glm/vec4.hpp>

#include "GLBuffer.h"
#include "GLProgram.h"

using namespace std;

GLBasicGeometryBase::GLBasicGeometryBase()
{
  // empty
}

GLBasicGeometryBase::~GLBasicGeometryBase()
{
  // empty
}

void GLBasicGeometryBase::SetVertexAttributes() const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", GetDimension());
}

void GLBasicGeometryBase::Buffer() const
{
  Bind();
  IssueBuffer();
}

void GLBasicGeometryBase::Draw(const mat4 &model) const
{
  Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLBasicGeometryBase::Bind() const { vertexArray.Bind(); }

void GLBasicGeometryBase::SetUniforms(const mat4 &model) const
{
  GLProgram::GetCurrentProgram()->SetUniform("u_model", model);
}
