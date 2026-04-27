#include "GLPoints.h"

#include "Utils.h"

using namespace std;

GLPoints::GLPoints()
{
  // empty
}

GLPoints::~GLPoints()
{
  // empty
}

void GLPoints::IssueDraw() const { GLDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()); }
