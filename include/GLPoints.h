#pragma once

#include "GLBufferedGeometry.h"

class GLPoints : public GL3DBufferedGeometry {
public:
  GLPoints();
  ~GLPoints() override;

  void Reserve(GLint n) { vertices.reserve(n); }
  void AddVertex(const vec3 &vertex) { vertices.push_back(vertex); }

protected:
  void IssueDraw() const override;
};
