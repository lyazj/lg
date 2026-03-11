#pragma once

#include "GLBufferedGeometry.h"

class GLBarnsleyFern : public GLBufferedGeometryD<2> {
public:
  GLBarnsleyFern(GLint points);
  ~GLBarnsleyFern() override;

  void Normalize();

protected:
  GLint points;

  void IssueDraw() const override;
};
