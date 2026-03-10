#include "GLBasicGeometry.h"

#include <glm/vec4.hpp>

#include "GLProgram.h"

using namespace std;

GLBasicGeometry::GLBasicGeometry(GLint d) : dimension(d)
{
  // empty
}

GLBasicGeometry::~GLBasicGeometry()
{
  // empty
}

void GLBasicGeometry::SetVertexAttributes() const
{
  vertexArray.Bind();
  vertexBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_position", dimension);
  GLProgram::SetVertexAttribute("a_color", glm::vec4(0.0, 0.0, 1.0, 1.0));
}

void GLBasicGeometry::Buffer() const
{
  Bind();
  IssueBuffer();
}

void GLBasicGeometry::Draw(const glm::mat4 &model) const
{
  Bind();
  SetUniforms(model);
  IssueDraw();
}

void GLBasicGeometry::Bind() const { vertexArray.Bind(); }

void GLBasicGeometry::SetUniforms(const glm::mat4 &model) const
{
  GLProgram::GetCurrentProgram()->SetUniform("u_model", model);
}
