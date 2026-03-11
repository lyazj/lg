#include "BarnsleyFern.h"

#include <algorithm>
#include <glm/mat2x2.hpp>
#include <iterator>

#include "Utils.h"

using namespace std;

namespace {

constexpr GLfloat cdf[4] = { 0.01f, 0.86f, 0.93f, 1.0f };
constexpr mat2 a[4] = {
  { { 0.0f, 0.0f }, { 0.0f, 0.16f } },
  { { 0.85f, -0.04f }, { 0.04f, 0.85f } },
  { { 0.2f, 0.23f }, { -0.26f, 0.22f } },
  { { -0.15f, 0.26f }, { 0.28f, 0.24f } },
};
constexpr vec2 b[4] = {
  { 0.0f, 0.0f },
  { 0.0f, 1.6f },
  { 0.0f, 1.6f },
  { 0.0f, 0.44f },
};

}  // namespace

BarnsleyFern::BarnsleyFern(GLint p) : points(p)
{
  vertices.reserve(points);
  vec2 x(0.0, 0.0);
  vertices.push_back(x);
  for(GLint i = 1; i < points; ++i) {
    size_t j = upper_bound(begin(cdf), end(cdf), drand48()) - begin(cdf);
    x = a[j] * x + b[j];
    vertices.push_back(x);
  }
}

BarnsleyFern::~BarnsleyFern()
{
  // empty
}

void BarnsleyFern::Normalize()
{
  GLfloat xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
  for(const vec2 &v : vertices) {
    xmin = min(xmin, v.x);
    xmax = max(xmax, v.x);
    ymin = min(ymin, v.y);
    ymax = max(ymax, v.y);
  }
  for(vec2 &v : vertices) {
    v.x = (v.x - xmin) / (xmax - xmin) * 2.0f - 1.0f;
    v.y = (v.y - ymin) / (ymax - ymin) * 2.0f - 1.0f;
  }
}

void BarnsleyFern::IssueDraw() const { GLDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()); }
