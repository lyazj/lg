#include "GLSierpinskiGasketChaos.h"

#include <math.h>

#include "Utils.h"

using namespace std;

GLSierpinskiGasketChaos::GLSierpinskiGasketChaos(GLint p) : points(p)
{
  vec2 triangle[3] = {
    { cosf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
    { cosf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
    { cosf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
  };

  vertices.reserve(points);
  vec2 x(0.0, 0.0);
  vertices.push_back(x);
  for(GLint i = 1; i < points; ++i) {
    int j = (int)lrand48() % 3;
    x = (x + triangle[j]) / 2.0f;
    vertices.push_back(x);
  }
}

GLSierpinskiGasketChaos::~GLSierpinskiGasketChaos()
{
  // empty
}

void GLSierpinskiGasketChaos::IssueDraw() const { GLDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()); }
