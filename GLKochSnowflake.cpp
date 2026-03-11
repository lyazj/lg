#include "GLKochSnowflake.h"

#include <math.h>

#include "Utils.h"

using namespace std;

static void Construct(vector<vec2> &vertices, const vec2 &a, const vec2 &b, GLint order)
{
  if(order-- == 0) return vertices.push_back(a);
  vec2 ab = b - a;
  vec2 c = (2.0f * a + b) / 3.0f;
  vec2 d = (a + 2.0f * b) / 3.0f;
  vec2 t = {
    cosf(60.0f * deg) * ab.x - sinf(60.0f * deg) * ab.y,
    sinf(60.0f * deg) * ab.x + cosf(60.0f * deg) * ab.y,
  };
  vec2 e = d - t / 3.0f;
  Construct(vertices, a, c, order);
  Construct(vertices, c, e, order);
  Construct(vertices, e, d, order);
  Construct(vertices, d, b, order);
}

GLKochSnowflake::GLKochSnowflake(GLint o) : order(o)
{
  vertices.reserve((size_t)(pow(4, order) * 3));
  vec2 a(cosf(0.0f * 2.0f * pi / 3.0f), sinf(0.0f * 2.0f * pi / 3.0f));
  vec2 b(cosf(1.0f * 2.0f * pi / 3.0f), sinf(1.0f * 2.0f * pi / 3.0f));
  vec2 c(cosf(2.0f * 2.0f * pi / 3.0f), sinf(2.0f * 2.0f * pi / 3.0f));
  Construct(vertices, a, b, order);
  Construct(vertices, b, c, order);
  Construct(vertices, c, a, order);
}

GLKochSnowflake::~GLKochSnowflake()
{
  // empty
}

void GLKochSnowflake::IssueDraw() const { GLDrawArrays(GL_LINE_LOOP, 0, (GLsizei)vertices.size()); }
