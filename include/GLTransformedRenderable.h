#pragma once

#include <glm/mat4x4.hpp>

#include "GLRenderable.h"

class GLTransformedRenderable : public GLRenderable {
public:
  GLTransformedRenderable(GLRenderablePtr r, const mat4 &m) : renderable(std::move(r)), model(m) { }
  ~GLTransformedRenderable() override = default;

  const GLRenderablePtr &GetRenderable() const { return renderable; }
  void SetRenderable(const GLRenderablePtr &r) { renderable = r; }
  const mat4 &GetModel() const { return model; }
  void SetModel(const mat4 &m) { model = m; }

  void IssueSetVertexAttributes(bool f) const override { return renderable->SetVertexAttributes(f); }
  void IssueBuffer(bool f) const override { return renderable->Buffer(f); }
  void IssueDraw(const mat4 &m) const override { return renderable->Draw(m * model); }

protected:
  GLRenderablePtr renderable;
  mat4 model;
};
