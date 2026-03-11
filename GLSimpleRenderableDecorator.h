#pragma once

#include "GLSimpleRenderable.h"

// GLSimpleRenderable decorator template: the identity.
class GLSimpleRenderableDecorator : public GLSimpleRenderable {
public:
  GLSimpleRenderableDecorator(GLSimpleRenderablePtr g) : geometry(g) { }
  ~GLSimpleRenderableDecorator() override = default;

  const GLSimpleRenderablePtr &GetGeometry() const { return geometry; }
  void SetGeometry(const GLSimpleRenderablePtr &g) { geometry = g; }

  void SetVertexAttributes() const override { geometry->SetVertexAttributes(); }
  void Buffer() const override { geometry->Buffer(); }
  void Draw(const mat4 &model) const override { geometry->Draw(model); }

  const GLVertexArray &GetVertexArray() const override { return geometry->GetVertexArray(); }
  const GLBuffer &GetVertexBuffer() const override { return geometry->GetVertexBuffer(); }

  GLint GetDimension() const override { return geometry->GetDimension(); }
  GLint GetNVertex() const override { return geometry->GetNVertex(); }
  void GetVertex(GLint i, vec2 &v) const override { geometry->GetVertex(i, v); }
  void GetVertex(GLint i, vec3 &v) const override { geometry->GetVertex(i, v); }

protected:
  GLSimpleRenderablePtr geometry;
};
