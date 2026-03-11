#pragma once

#include "GLBufferedGeometry.h"

class GLSmiley : public GL2DBufferedGeometry {
public:
  GLSmiley(GLfloat radius, GLint segments);
  ~GLSmiley() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
