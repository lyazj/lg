#pragma once

#include "GLBufferedGeometry.h"

class GLRectangle : public GLBufferedGeometryD<2> {
public:
  GLRectangle(GLfloat width, GLfloat height);
  ~GLRectangle() override;

protected:
  GLfloat width, height;

  void IssueDraw() const override;
};
