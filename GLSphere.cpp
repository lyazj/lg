#include "GLSphere.h"

#include <math.h>

#include <algorithm>
#include <cstdint>

#include "Utils.h"

using namespace std;

GLSphere::GLSphere(GLfloat r, GLint sl, GLint st)
    : radius(r), slices(max<GLint>(3, sl)), stacks(max<GLint>(2, st)), elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  vertices.reserve(2 + slices * (stacks - 1));
  elements.reserve(2 * (slices + 1) + (stacks - 2) * (2 * slices + 2));

  auto addVertex = [this](GLfloat phi, GLfloat lambda) {
    GLfloat z = radius * sinf(phi), rho = radius * cosf(phi);
    GLfloat x = rho * cosf(lambda), y = rho * sinf(lambda);
    vertices.emplace_back(x, y, z);
  };

  // Generate vertices.
  addVertex(90.0f * deg, 0.0f * deg);
  for(GLint istack = 0; istack + 1 < stacks; ++istack) {
    GLfloat phi = (90.0f - 180.0f * GLfloat(istack + 1) / GLfloat(stacks)) * deg;
    for(GLint islice = 0; islice < slices; ++islice) {
      GLfloat lambda = (360.0f * GLfloat(islice) / GLfloat(slices)) * deg;
      addVertex(phi, lambda);
    }
  }
  addVertex(-90.0f * deg, 0.0f * deg);
  GLint nVertex = (GLint)vertices.size();

  // Render the north pole as a triangle fan.
  elements.push_back(0);
  for(GLint islice = 0; islice < slices; ++islice) elements.push_back(islice + 1);
  elements.push_back(1);

  // Render the middle stacks as triangle strips.
  for(GLint istack = 1, ibase = 1; istack + 1 < stacks; ++istack, ibase += slices) {
    for(GLint islice = 0; islice < slices; ++islice) {
      elements.push_back(ibase + islice);
      elements.push_back(ibase + islice + slices);
    }
    elements.push_back(ibase);
    elements.push_back(ibase + slices);
  }

  // Render the south pole as a triangle fan.
  elements.push_back(nVertex - 1);
  for(GLint islice = 0; islice < slices; ++islice) elements.push_back(nVertex - 2 - islice);
  elements.push_back(nVertex - 2);
}

GLSphere::~GLSphere()
{
  // empty
}

void GLSphere::IssueBuffer() const
{
  GLBasicGeometry::IssueBuffer();
  elementBuffer.Buffer(elements);
}

void GLSphere::IssueDraw() const
{
  uintptr_t offset = 0;

  GLDrawElements(GL_TRIANGLE_FAN, slices + 2, GL_UNSIGNED_INT, reinterpret_cast<void *>(offset));
  offset += (uintptr_t)(slices + 2) * sizeof(GLuint);

  for(GLint istack = 1; istack + 1 < stacks; ++istack) {
    GLDrawElements(GL_TRIANGLE_STRIP, 2 * slices + 2, GL_UNSIGNED_INT, reinterpret_cast<void *>(offset));
    offset += (uintptr_t)(2 * slices + 2) * sizeof(GLuint);
  }

  GLDrawElements(GL_TRIANGLE_FAN, slices + 2, GL_UNSIGNED_INT, reinterpret_cast<void *>(offset));
}
