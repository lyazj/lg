#pragma once

#include "GLElementGeometry.h"

class BarnsleyFern : public GLElementGeometryD<2> {
public:
  BarnsleyFern(GLint points);
  ~BarnsleyFern() override;

  void Normalize();

protected:
  GLint points;

  void IssueDraw() const override;
};
