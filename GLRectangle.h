#pragma once

#include "GLBufferedGeometry.h"

class GLRectangle : public GL2DBufferedGeometry {
public:
  GLRectangle(GLfloat width, GLfloat height);
  ~GLRectangle() override;

protected:
  GLfloat width, height;

  void IssueDraw() const override;
};
