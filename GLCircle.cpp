#include "GLCircle.h"

#include <math.h>

#include "Utils.h"

using namespace std;

GLCircle::GLCircle(GLfloat r, GLint s) : GLBasicGeometry(2), radius(r), segments(s)
{
  vertices.reserve(segments + 2);
  vertices.emplace_back(0.0f, 0.0f);
  for(GLint i = 0; i < segments + 1; ++i) {
    GLfloat angle = (GLfloat)i * 2.0f * pi / (GLfloat)segments;
    vertices.emplace_back(radius * cos(angle), radius * sin(angle));
  }
  vertices.emplace_back(radius, 0.0f);
}

GLCircle::~GLCircle()
{
  // empty
}

void GLCircle::IssueBuffer() const { vertexBuffer.Buffer(vertices); }

void GLCircle::IssueDraw() const { GLDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)vertices.size()); }
