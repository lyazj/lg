#pragma once

#include "GLBufferedGeometry.h"

class GLKochSnowflake : public GLBufferedGeometryD<2> {
public:
  GLKochSnowflake(GLint order);
  ~GLKochSnowflake() override;

protected:
  GLint order;

  void IssueDraw() const override;
};
