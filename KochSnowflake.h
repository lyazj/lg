#pragma once

#include "GLElementGeometry.h"

class KochSnowflake : public GLElementGeometryD<2> {
public:
  KochSnowflake(GLint order);
  ~KochSnowflake() override;

protected:
  GLint order;

  void IssueDraw() const override;
};
