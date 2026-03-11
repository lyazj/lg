#include "GLSierpinskiGasket.h"

#include <math.h>

#include "Utils.h"

using namespace std;

static void Construct(vector<vec2> &vertices, const vec2 &a, const vec2 &b, const vec2 &c, GLint order, GLfloat noise)
{
  if(order-- == 0) {
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    return;
  }
  noise /= 2.0f;
  vec2 ab = (a + b) / 2.0f + vec2(drand48() * noise, drand48() * noise);
  vec2 bc = (b + c) / 2.0f + vec2(drand48() * noise, drand48() * noise);
  vec2 ca = (c + a) / 2.0f + vec2(drand48() * noise, drand48() * noise);
  Construct(vertices, a, ab, ca, order, noise);
  Construct(vertices, b, bc, ab, order, noise);
  Construct(vertices, c, ca, bc, order, noise);
}

GLSierpinskiGasket::GLSierpinskiGasket(GLint o, GLfloat f) : order(o), noise(f)
{
  vertices.reserve((size_t)pow(3, order + 1));
  vec2 a(cosf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  vec2 b(cosf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  vec2 c(cosf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  Construct(vertices, a, b, c, order, noise);
}

GLSierpinskiGasket::~GLSierpinskiGasket()
{
  // empty
}

void GLSierpinskiGasket::IssueDraw() const { GLDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size()); }
