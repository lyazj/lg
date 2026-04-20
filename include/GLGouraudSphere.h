#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLGouraudSphere : public GL3DBufferedGeometry {
public:
  GLGouraudSphere(GLfloat radius, GLint order);
  ~GLGouraudSphere() override;

protected:
  GLfloat radius;
  GLint order;
  GLBuffer normalBuffer;
  GLBuffer elementBuffer;
  std::vector<vec3> normals;
  std::vector<GLuint> elements;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer() const override;
  void IssueDraw() const override;
};
