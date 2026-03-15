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

  const GLSimpleRenderablePtr &GetGeometry() const { return renderable; }
  void SetGeometry(const GLSimpleRenderablePtr &r) { renderable = r; }

  void SetVertexAttributes() const override { renderable->SetVertexAttributes(); }
  void Buffer() const override { renderable->Buffer(); }
  void Draw(const mat4 &model) const override;

  const GLVertexArray &GetVertexArray() const override { return renderable->GetVertexArray(); }
  const GLBuffer &GetVertexBuffer() const override { return renderable->GetVertexBuffer(); }

  GLint GetDimension() const override { return renderable->GetDimension(); }
  GLint GetNVertex() const override { return renderable->GetNVertex(); }
  void GetVertex(GLint i, vec2 &v) const override { renderable->GetVertex(i, v); }
  void GetVertex(GLint i, vec3 &v) const override { renderable->GetVertex(i, v); }

protected:
  GLSimpleRenderablePtr renderable;
  GLProgramPtr program;
};
