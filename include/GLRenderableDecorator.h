#pragma once

#include "GLRenderable.h"

// GLRenderable decorator template: the identity.
class GLRenderableDecorator : public GLRenderable {
public:
  GLRenderableDecorator(GLRenderablePtr r, GLProgramPtr p) : renderable(r), program(p)
  {
    // empty
  }
  ~GLRenderableDecorator() override = default;

  const GLProgramPtr &GetProgram() const { return program; }
  void SetProgram(const GLProgramPtr &p) { program = p; }

  const GLRenderablePtr &GetRenderable() const { return renderable; }
  void SetRenderable(const GLRenderablePtr &r) { renderable = r; }

  void Draw(const mat4 &model) const override final;

protected:
  GLRenderablePtr renderable;
  GLProgramPtr program;

  void IssueSetVertexAttributes(bool f) const override { renderable->SetVertexAttributes(f); }
  void IssueBuffer(bool f) const override { renderable->Buffer(f); }
  virtual void IssueDraw(const mat4 &model) const { renderable->Draw(model); }
};
