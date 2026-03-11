#pragma once

#include <glm/mat4x4.hpp>

#include "GLRenderable.h"

class GLTransformedRenderable : public GLRenderable {
public:
  GLTransformedRenderable(GLRenderablePtr r, mat4 m) : renderable(std::move(r)), model(m) { }
  ~GLTransformedRenderable() override = default;

  const GLRenderablePtr &GetGeometry() const { return renderable; }
  void SetGeometry(const GLRenderablePtr &r) { renderable = r; }
  const mat4 &GetModel() const { return model; }
  void SetModel(const mat4 &m) { model = m; }

  void SetVertexAttributes() const override { return renderable->SetVertexAttributes(); }
  void Buffer() const override { return renderable->Buffer(); }
  void Draw(const mat4 &m) const override { return renderable->Draw(m * model); }

protected:
  GLRenderablePtr renderable;
  mat4 model;
};
