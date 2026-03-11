#pragma once

#include "GLElementGeometry.h"

class GLSmell : public GLElementGeometryD<2> {
public:
  GLSmell(GLfloat radius, GLint segments);
  ~GLSmell() override;

protected:
  GLfloat radius;
  GLint segments;

  void IssueDraw() const override;
};
