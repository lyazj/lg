#pragma once

#include "GLBufferedGeometry.h"

class GLTriangleStrip : public GL3DBufferedGeometry {
public:
  GLTriangleStrip();
  ~GLTriangleStrip() override;

  void Reserve(GLint n) { vertices.reserve(n); }
  void AddVertex(const vec3 &vertex) { vertices.push_back(vertex); }

protected:
  void IssueDraw() const override;
};
