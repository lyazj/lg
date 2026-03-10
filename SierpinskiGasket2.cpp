#include "SierpinskiGasket2.h"

#include <math.h>

#include "Utils.h"

using namespace std;

SierpinskiGasket2::SierpinskiGasket2(GLint p) : GLBasicGeometry(2), points(p)
{
  glm::vec2 triangle[3] = {
    { cosf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
    { cosf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
    { cosf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f) },
  };

  vertices.reserve(points);
  glm::vec2 x(0.0, 0.0);
  vertices.push_back(x);
  for(GLint i = 1; i < points; ++i) {
    int j = (int)lrand48() % 3;
    x = (x + triangle[j]) / 2.0f;
    vertices.push_back(x);
  }
}

SierpinskiGasket2::~SierpinskiGasket2()
{
  // empty
}

void SierpinskiGasket2::IssueBuffer() const { vertexBuffer.Buffer(vertices); }

void SierpinskiGasket2::IssueDraw() const { GLDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()); }
