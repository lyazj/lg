#include "GLTriangle.h"

#include "Utils.h"

using namespace std;

GLTriangle::GLTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3) { vertices = { v1, v2, v3 }; }

GLTriangle::~GLTriangle()
{
  // empty
}

void GLTriangle::IssueDraw() const { GLDrawArrays(GL_TRIANGLES, 0, 3); }
