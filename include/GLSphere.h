#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLSphere : public GL3DBufferedGeometry {
public:
  GLSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLSphere() override;

protected:
  GLfloat radius;
  GLint slices, stacks;
  GLBuffer texCoordBuffer;
  GLBuffer elementBuffer;
  std::vector<vec2> texCoords;
  std::vector<GLuint> elements;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer() const override;
  void IssueDraw() const override;
};
