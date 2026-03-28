#include "GLSphere.h"

#include <math.h>

#include <algorithm>
#include <glm/geometric.hpp>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLSphere::GLSphere(GLfloat r, GLint sl, GLint st)
    : radius(r), slices(max<GLint>(3, sl)), stacks(max<GLint>(2, st)), elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  vertices.reserve((slices + 1) * (stacks + 1));
  texCoords.reserve((slices + 1) * (stacks + 1));
  elements.reserve(2 * (slices + 1) * stacks);

  // We generate slices + 1 vertices per latitude (including the poles) to ensure
  // correct UV mapping across the seam (and at the poles).
  // Sines and cosines are not cached for simplicity.
  for(GLint istack = 0; istack <= stacks; ++istack) {
    GLfloat v = 1.0f - GLfloat(istack) / GLfloat(stacks);
    GLfloat phi = (v - 0.5f) * pi;
    for(GLint islice = 0; islice <= slices; ++islice) {
      GLfloat u = GLfloat(islice) / GLfloat(slices);
      GLfloat lambda = u * (2.0f * pi);
      GLfloat z = radius * sinf(phi), rho = radius * cosf(phi);
      GLfloat x = rho * cosf(lambda), y = rho * sinf(lambda);
      vertices.emplace_back(x, y, z);
      texCoords.emplace_back(u, v);
    }
  }

  for(GLint istack = 0; istack < stacks; ++istack) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      elements.push_back(istack * (slices + 1) + islice);
      elements.push_back((istack + 1) * (slices + 1) + islice);
    }
  }
}

GLSphere::~GLSphere()
{
  // empty
}

void GLSphere::SetVertexAttributes() const
{
  GL3DBufferedGeometry::SetVertexAttributes();
  vertexBuffer.Bind();  // reused: centered at the origin, normalized by shaders
  GLProgram::SetVertexAttributePointer("a_normal", 3);
  texCoordBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_texCoord0", 2);
}

void GLSphere::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  texCoordBuffer.Buffer(texCoords);
  elementBuffer.Buffer(elements);
}

void GLSphere::IssueDraw() const
{
  GLuint *offset = nullptr;
  for(GLint istack = 0; istack < stacks; ++istack) {
    GLDrawElements(GL_TRIANGLE_STRIP, 2 * (slices + 1), GL_UNSIGNED_INT, offset);
    offset += 2 * (slices + 1);
  }
}
