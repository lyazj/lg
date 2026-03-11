#pragma once

#include "GLBufferedGeometry.h"

class GLSmiley : public GLBufferedGeometryD<2> {
public:
  GLSmiley(GLfloat radius, GLint segments);
  ~GLSmiley() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
