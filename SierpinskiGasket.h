#pragma once

#include "GLElementGeometry.h"

class SierpinskiGasket : public GLElementGeometryD<2> {
public:
  SierpinskiGasket(GLint order, GLfloat noise = 0.0f);
  ~SierpinskiGasket() override;

protected:
  GLint order;
  GLfloat noise;

  void IssueDraw() const override;
};
