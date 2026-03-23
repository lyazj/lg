#include "GLSphere.h"

#include <math.h>

#include <algorithm>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLSphere::GLSphere(GLfloat r, GLint sl, GLint st)
    : radius(r), slices(max<GLint>(3, sl)), stacks(max<GLint>(2, st)), elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  vertices.reserve(2 + (slices + 1) * (stacks - 1));
  texCoords.reserve(2 + (slices + 1) * (stacks - 1));
  elements.reserve(2 * (slices + 1) + (stacks - 2) * (2 * slices + 2));

  auto addVertex = [this](GLfloat phi, GLfloat lambda) {
    GLfloat z = radius * sinf(phi), rho = radius * cosf(phi);
    GLfloat x = rho * cosf(lambda), y = rho * sinf(lambda);
    vertices.emplace_back(x, y, z);
    texCoords.emplace_back(lambda / (2.0f * pi), phi / pi + 0.5f);
  };

  // Generate vertices.
  addVertex(90.0f * deg, 0.0f * deg);
  for(GLint istack = 0; istack + 1 < stacks; ++istack) {
    GLfloat phi = (90.0f - 180.0f * GLfloat(istack + 1) / GLfloat(stacks)) * deg;
    for(GLint islice = 0; islice <= slices; ++islice) {
      GLfloat lambda = (360.0f * GLfloat(islice) / GLfloat(slices)) * deg;
      addVertex(phi, lambda);
    }
  }
  addVertex(-90.0f * deg, 0.0f * deg);
  GLint nVertex = (GLint)vertices.size();

  // Render the north pole as a triangle fan.
  elements.push_back(0);
  for(GLint islice = 0; islice <= slices; ++islice) elements.push_back(islice + 1);

  // Render the middle stacks as triangle strips.
  for(GLint istack = 1, ibase = 1; istack + 1 < stacks; ++istack, ibase += slices + 1) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      elements.push_back(ibase + islice);
      elements.push_back(ibase + islice + slices + 1);
    }
  }

  // Render the south pole as a triangle fan.
  elements.push_back(nVertex - 1);
  for(GLint islice = 0; islice <= slices; ++islice) elements.push_back(nVertex - 2 - islice);
}

GLSphere::~GLSphere()
{
  // empty
}

void GLSphere::SetVertexAttributes() const
{
  GL3DBufferedGeometry::SetVertexAttributes();
  normalBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_normal", 3);
  texCoordBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_texCoord0", 2);
}

void GLSphere::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  normalBuffer.Buffer(vertices);
  texCoordBuffer.Buffer(texCoords);
  elementBuffer.Buffer(elements);
}

void GLSphere::IssueDraw() const
{
  GLuint *offset = nullptr;

  GLDrawElements(GL_TRIANGLE_FAN, slices + 2, GL_UNSIGNED_INT, offset);
  offset += slices + 2;

  for(GLint istack = 1; istack + 1 < stacks; ++istack) {
    GLDrawElements(GL_TRIANGLE_STRIP, 2 * slices + 2, GL_UNSIGNED_INT, offset);
    offset += 2 * slices + 2;
  }

  GLDrawElements(GL_TRIANGLE_FAN, slices + 2, GL_UNSIGNED_INT, offset);
  offset += slices + 2;
}
