#pragma once

#include "GLBasicGeometry.h"

class GLCircle : public GLBasicGeometry<2> {
public:
  GLCircle(GLfloat radius, GLint segments);
  ~GLCircle() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
