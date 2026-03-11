#pragma once

#include "GLBasicGeometry.h"

class KochSnowflake : public GLBasicGeometry<2> {
public:
  KochSnowflake(GLint order);
  ~KochSnowflake() override;

protected:
  GLint order;

  void IssueDraw() const override;
};
