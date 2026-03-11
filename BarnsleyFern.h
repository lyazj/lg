#pragma once

#include "GLBasicGeometry.h"

class BarnsleyFern : public GLBasicGeometry<2> {
public:
  BarnsleyFern(GLint points);
  ~BarnsleyFern() override;

  void Normalize();

protected:
  GLint points;

  void IssueDraw() const override;
};
