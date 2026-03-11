#pragma once

#include "GLBufferedGeometry.h"

class GLTriangle : public GL3DBufferedGeometry {
public:
  GLTriangle(const vec3 &v1, const vec3 &v2, const vec3 &v3);
  ~GLTriangle() override;

protected:
  void IssueDraw() const override;
};
