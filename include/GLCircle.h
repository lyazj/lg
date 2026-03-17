#pragma once

#include "GLBufferedGeometry.h"

class GLCircle : public GL2DBufferedGeometry {
public:
  GLCircle(GLfloat radius, GLint segments);
  ~GLCircle() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
