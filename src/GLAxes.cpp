#include "GLAxes.h"

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLAxes::GLAxes(GLfloat x, GLfloat y, GLfloat z, const vec4 &xcolor, const vec4 &ycolor, const vec4 &zcolor)
{
  vertices = {
    { 0.0f, 0.0f, 0.0f },
    { x, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, y, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, z },
  };
  colors = { xcolor, xcolor, ycolor, ycolor, zcolor, zcolor };
}

GLAxes::~GLAxes()
{
  // empty
}

void GLAxes::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
}

void GLAxes::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  colorBuffer.Buffer(colors);
}

void GLAxes::IssueDraw() const { GLDrawArrays(GL_LINES, 0, 6); }
