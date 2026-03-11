#pragma once

#include "GLBasicGeometry.h"

class GLSmell : public GLBasicGeometry<2> {
public:
  GLSmell(GLfloat radius, GLint segments);
  ~GLSmell() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
