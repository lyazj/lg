#include "GLSmell.h"

#include <math.h>

#include <algorithm>

#include "Utils.h"

using namespace std;

GLSmell::GLSmell(GLfloat r, GLint s) : GLBasicGeometry(2), radius(r), segments(max<GLint>(3, s))
{
  vertices.reserve((segments + 1) * 4);

  // Head.
  for(GLint i = 0; i <= segments; ++i) {
    GLfloat angle = (GLfloat)i * 2.0f * pi / (GLfloat)segments;
    vertices.emplace_back(radius * cosf(angle), radius * sinf(angle));
  }

  // ChatGPT.
  GLfloat eyeRadius = radius * 0.1f;
  GLfloat eyeOffsetX = radius * 0.3f;
  GLfloat eyeOffsetY = radius * 0.3f;

  // Eyes.
  for(GLint e = 0; e < 2; ++e) {
    GLfloat cx = (e == 0 ? -eyeOffsetX : eyeOffsetX), cy = eyeOffsetY;
    for(GLint i = 0; i <= segments; ++i) {
      GLfloat angle = (GLfloat)i * 2.0f * pi / (GLfloat)segments;
      vertices.emplace_back(cx + eyeRadius * cosf(angle), cy + eyeRadius * sinf(angle));
    }
  }

  // ChatGPT.
  GLfloat mouthRadius = radius * 0.5f;
  GLfloat mouthOffsetY = -radius * 0.2f;

  // Mouth.
  for(GLint i = 0; i <= segments; ++i) {
    GLfloat angle = (GLfloat)i * pi / (GLfloat)segments + pi;
    vertices.emplace_back(mouthRadius * cosf(angle), mouthOffsetY + mouthRadius * sinf(angle));
  }
}

GLSmell::~GLSmell()
{
  // empty
}

void GLSmell::IssueBuffer() const { vertexBuffer.Buffer(vertices); }

void GLSmell::IssueDraw() const
{
  GLint offset = 0;
  for(GLint i = 0; i < 4; ++i) {
    GLDrawArrays(GL_LINE_STRIP, offset, segments + 1);
    offset += segments + 1;
  }
}
