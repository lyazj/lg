#include "GLFrustumLateral.h"

#include <math.h>

#include <algorithm>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLFrustumLateral::GLFrustumLateral(GLfloat tr, GLfloat br, GLfloat h, GLint s)
    : topRadius(tr), bottomRadius(br), height(h), slices(max<GLint>(3, s))
{
  vertices.reserve(2 * (slices + 1));
  normals.reserve(2 * (slices + 1));
  texCoords.reserve(2 * (slices + 1));

  GLfloat halfConeAngle = atanf((bottomRadius - topRadius) / height);
  GLfloat sinHalfConeAngle = sinf(halfConeAngle), cosHalfConeAngle = cosf(halfConeAngle);

  for(GLint islice = 0; islice <= slices; ++islice) {
    GLfloat u = GLfloat(islice) / GLfloat(slices);
    GLfloat lambda = u * (2.0f * pi);
    GLfloat sinLambda = sinf(lambda), cosLambda = cosf(lambda);
    for(GLint istack = 0; istack <= 1; ++istack) {
      GLfloat v = 1.0f - GLfloat(istack);
      GLfloat z = (v - 0.5f) * height, rho = bottomRadius + v * (topRadius - bottomRadius);
      GLfloat x = rho * cosLambda, y = rho * sinLambda;
      vertices.emplace_back(x, y, z);
      z = sinHalfConeAngle, rho = cosHalfConeAngle;
      x = rho * cosLambda, y = rho * sinLambda;
      normals.emplace_back(x, y, z);
      texCoords.emplace_back(u, v);
    }
  }
}

GLFrustumLateral::~GLFrustumLateral()
{
  // empty
}

void GLFrustumLateral::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  normalBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_normal", 3);
  texCoordBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_texCoord0", 2);
}

void GLFrustumLateral::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  normalBuffer.Buffer(normals);
  texCoordBuffer.Buffer(texCoords);
}

void GLFrustumLateral::IssueDraw() const { GLDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)vertices.size()); }
