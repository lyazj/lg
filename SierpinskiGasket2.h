#pragma once

#include "GLElementGeometry.h"

class SierpinskiGasket2 : public GLElementGeometryD<2> {
public:
  SierpinskiGasket2(GLint points);
  ~SierpinskiGasket2() override;

protected:
  GLint points;

  void IssueDraw() const override;
};
