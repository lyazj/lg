#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLFrustumLateral : public GL3DBufferedGeometry {
public:
  GLFrustumLateral(GLfloat topRadius, GLfloat bottomRadius, GLfloat height, GLint slices);
  ~GLFrustumLateral() override;

protected:
  GLfloat topRadius, bottomRadius, height;
  GLint slices;
  GLBuffer normalBuffer;
  GLBuffer texCoordBuffer;
  std::vector<vec3> normals;
  std::vector<vec2> texCoords;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer() const override;
  void IssueDraw() const override;
};
