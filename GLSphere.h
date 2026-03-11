#pragma once

#include "GLBasicGeometry.h"
#include "GLBuffer.h"

class GLSphere : public GLBasicGeometry<3> {
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
