#include "GLAxes.h"

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLAxes::GLAxes(
    GLfloat x, GLfloat y, GLfloat z, const glm::vec4 &xcolor, const glm::vec4 &ycolor, const glm::vec4 &zcolor)
    : GLBasicGeometry(3),
      vertices{
        { 0.0f, 0.0f, 0.0f },
        { x, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, y, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, z },
      },
      colors{ xcolor, xcolor, ycolor, ycolor, zcolor, zcolor }
{
  // empty
}

GLAxes::~GLAxes()
{
  // empty
}

void GLAxes::SetVertexAttributes() const
{
  GLBasicGeometry::SetVertexAttributes();
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
}

void GLAxes::IssueBuffer() const
{
  vertexBuffer.Buffer(vertices);
  colorBuffer.Buffer(colors);
}

void GLAxes::IssueDraw() const { GLDrawArrays(GL_LINES, 0, 6); }
