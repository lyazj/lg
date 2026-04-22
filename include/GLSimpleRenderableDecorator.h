#pragma once

#include "GLSimpleRenderable.h"

// GLSimpleRenderable decorator template: the identity.
class GLSimpleRenderableDecorator : public GLSimpleRenderable {
public:
  GLSimpleRenderableDecorator(GLSimpleRenderablePtr r, GLProgramPtr p) : renderable(r), program(p)
  {
    // empty
  }
  ~GLSimpleRenderableDecorator() override = default;

  const GLProgramPtr &GetProgram() const { return program; }
  void SetProgram(const GLProgramPtr &p) { program = p; }

  const GLSimpleRenderablePtr &GetRenderable() const { return renderable; }
  void SetRenderable(const GLSimpleRenderablePtr &r) { renderable = r; }

  using GLSimpleRenderable::GetVertexArray;
  using GLSimpleRenderable::GetVertexBuffer;
  const GLVertexArray &GetVertexArray() const override final { return renderable->GetVertexArray(); }
  const GLBuffer &GetVertexBuffer() const override final { return renderable->GetVertexBuffer(); }

  GLint GetDimension() const override final { return renderable->GetDimension(); }
  GLint GetNVertex() const override final { return renderable->GetNVertex(); }
  void GetVertex(GLint i, vec2 &v) const override final { renderable->GetVertex(i, v); }
  void GetVertex(GLint i, vec3 &v) const override final { renderable->GetVertex(i, v); }

protected:
  GLSimpleRenderablePtr renderable;
  GLProgramPtr program;

  void IssueSetVertexAttributes(bool f) const override { renderable->SetVertexAttributes(f); }
  void IssueBuffer(bool f) const override { renderable->Buffer(f); }
  void IssueDraw(const mat4 &model) const override final;
  virtual void DecoratorDraw(const mat4 &model) const { renderable->Draw(model); }
};
