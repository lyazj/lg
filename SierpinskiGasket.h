#pragma once

#include "GLBasicGeometry.h"

class SierpinskiGasket : public GLBasicGeometry<2> {
public:
  SierpinskiGasket(GLint order, GLfloat noise = 0.0f);
  ~SierpinskiGasket() override;

protected:
  GLint order;
  GLfloat noise;

  void IssueDraw() const override;
};
