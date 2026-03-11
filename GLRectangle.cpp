#include "GLRectangle.h"

#include "Utils.h"

using namespace std;

GLRectangle::GLRectangle(GLfloat w, GLfloat h) : width(w), height(h)
{
  w /= 2.0f, h /= 2.0f;
  vertices = { { -w, h }, { -w, -h }, { w, h }, { w, -h } };
}

GLRectangle::~GLRectangle()
{
  // empty
}

void GLRectangle::IssueDraw() const { GLDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size()); }
