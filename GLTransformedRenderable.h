#pragma once

#include <glm/mat4x4.hpp>

#include "GLRenderable.h"

class GLTransformedRenderable : public GLRenderable {
public:
  GLTransformedRenderable(GLRenderablePtr g, mat4 m) : geometry(std::move(g)), model(m) { }
  ~GLTransformedRenderable() override = default;

  const GLRenderablePtr &GetGeometry() const { return geometry; }
  void SetGeometry(const GLRenderablePtr &g) { geometry = g; }
  const mat4 &GetModel() const { return model; }
  void SetModel(const mat4 &m) { model = m; }

  void SetVertexAttributes() const override { return geometry->SetVertexAttributes(); }
  void Buffer() const override { return geometry->Buffer(); }
  void Draw(const mat4 &m) const override { return geometry->Draw(m * model); }

protected:
  GLRenderablePtr geometry;
  mat4 model;
};
