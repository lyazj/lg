#pragma once

#include "GLBasicGeometry.h"

class GLTriangle : public GLBasicGeometry<3> {
public:
  GLTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);
  ~GLTriangle() override;

protected:
  void IssueDraw() const override;
};
