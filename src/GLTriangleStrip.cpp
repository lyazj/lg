#include "GLTriangleStrip.h"

#include "Utils.h"

using namespace std;

GLTriangleStrip::GLTriangleStrip()
{
  // empty
}

GLTriangleStrip::~GLTriangleStrip()
{
  // empty
}

void GLTriangleStrip::IssueDraw() const { GLDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size()); }
