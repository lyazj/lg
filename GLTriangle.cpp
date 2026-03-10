#include "GLTriangle.h"

#include "Utils.h"

using namespace std;

GLTriangle::GLTriangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3)
    : GLBasicGeometry(3), vertices{ v1, v2, v3 }
{
  // empty
}

GLTriangle::~GLTriangle()
{
  // empty
}

void GLTriangle::IssueBuffer() const { vertexBuffer.Buffer(vertices); }

void GLTriangle::IssueDraw() const { GLDrawArrays(GL_TRIANGLES, 0, 3); }
