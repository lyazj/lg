#pragma once

#include "GLBufferedGeometry.h"

class GLKochSnowflake : public GL2DBufferedGeometry {
public:
  GLKochSnowflake(GLint order);
  ~GLKochSnowflake() override;

protected:
  GLint order;

  void IssueDraw() const override;
};
