#pragma once

#include "GLBufferedGeometry.h"

class GLBarnsleyFern : public GL2DBufferedGeometry {
public:
  GLBarnsleyFern(GLint points);
  ~GLBarnsleyFern() override;

  void Normalize();

protected:
  GLint points;

  void IssueDraw() const override;
};
