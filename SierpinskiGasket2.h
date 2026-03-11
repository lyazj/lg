#pragma once

#include "GLBasicGeometry.h"

class SierpinskiGasket2 : public GLBasicGeometry<2> {
public:
  SierpinskiGasket2(GLint points);
  ~SierpinskiGasket2() override;

protected:
  GLint points;

  void IssueDraw() const override;
};
