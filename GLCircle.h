#pragma once

#include "GLElementGeometry.h"

class GLCircle : public GLElementGeometryD<2> {
public:
  GLCircle(GLfloat radius, GLint segments);
  ~GLCircle() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
