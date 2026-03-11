#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLSphere : public GLBufferedGeometryD<3> {
public:
  GLSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLSphere() override;

protected:
  GLfloat radius;
  GLint slices, stacks;
  GLBuffer elementBuffer;
  std::vector<GLuint> elements;

  void IssueBuffer() const override;
  void IssueDraw() const override;
};
