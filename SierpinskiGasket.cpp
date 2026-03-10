#include "SierpinskiGasket.h"

#include <math.h>

#include "Utils.h"

using namespace std;

static void Construct(
    vector<glm::vec2> &vertices, const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c, GLint order, GLfloat noise)
{
  if(order-- == 0) {
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    return;
  }
  noise /= 2.0f;
  glm::vec2 ab = (a + b) / 2.0f + glm::vec2(drand48() * noise, drand48() * noise);
  glm::vec2 bc = (b + c) / 2.0f + glm::vec2(drand48() * noise, drand48() * noise);
  glm::vec2 ca = (c + a) / 2.0f + glm::vec2(drand48() * noise, drand48() * noise);
  Construct(vertices, a, ab, ca, order, noise);
  Construct(vertices, b, bc, ab, order, noise);
  Construct(vertices, c, ca, bc, order, noise);
}

SierpinskiGasket::SierpinskiGasket(GLint o, GLfloat f) : GLBasicGeometry(2), order(o), noise(f)
{
  vertices.reserve((size_t)pow(3, order + 1));
  glm::vec2 a(cosf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(0.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  glm::vec2 b(cosf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(1.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  glm::vec2 c(cosf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f), sinf(2.0f * 2.0f * pi / 3.0f + pi / 2.0f));
  Construct(vertices, a, b, c, order, noise);
}

SierpinskiGasket::~SierpinskiGasket()
{
  // empty
}

void SierpinskiGasket::IssueBuffer() const { vertexBuffer.Buffer(vertices); }

void SierpinskiGasket::IssueDraw() const { GLDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size()); }
