#pragma once

#include <glm/mat4x4.hpp>

#include "GLGeometry.h"

class GLTransformedGeometry : public GLGeometry {
public:
  GLTransformedGeometry(GLGeometryPtr g, mat4 m) : geometry(g), model(m) { }
  ~GLTransformedGeometry() override = default;

  const GLGeometryPtr &GetGeometry() const { return geometry; }
  void SetGeometry(const GLGeometryPtr &g) { geometry = g; }
  const mat4 &GetModel() const { return model; }
  void SetModel(const mat4 &m) { model = m; }

  void SetVertexAttributes() const override { return geometry->SetVertexAttributes(); }
  void Buffer() const override { return geometry->Buffer(); }
  void Draw(const mat4 &m) const override { return geometry->Draw(m * model); }

protected:
  GLGeometryPtr geometry;
  mat4 model;
};
