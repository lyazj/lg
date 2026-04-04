#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderable.h"
#include "GLVertexArray.h"

class GLBufferedGeometry : public GLSimpleRenderable {
public:
  GLBufferedGeometry();
  ~GLBufferedGeometry() override;

  const GLVertexArray &GetVertexArray() const override { return vertexArray; }
  const GLBuffer &GetVertexBuffer() const override { return vertexBuffer; }

protected:
  GLVertexArray vertexArray;
  GLBuffer vertexBuffer;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer(bool force) const override;
  void IssueDraw(const mat4 &model) const override;

  virtual void SetUniforms(const mat4 &model) const;
  virtual void IssueBuffer() const = 0;
  virtual void IssueDraw() const = 0;
};

template<GLint D>
class GLBufferedGeometryD : public GLBufferedGeometry {
public:
  GLBufferedGeometryD() = default;
  ~GLBufferedGeometryD() override = default;

  GLint GetDimension() const override final { return D; }
  GLint GetNVertex() const override final { return (GLint)vertices.size(); }
  void GetVertex(GLint i, vec2 &v) const override final;
  void GetVertex(GLint i, vec3 &v) const override final;
  const vec<D> &GetVertex(GLint i) const { return vertices[i]; }

protected:
  std::vector<vec<D>> vertices;
  void IssueBuffer() const override { vertexBuffer.Buffer(vertices); }
};

template<GLint D>
inline void GLBufferedGeometryD<D>::GetVertex(GLint i, vec2 &v) const
{
  if constexpr(D == 2) {
    v = vertices[i];
  } else {
    v = vec2(vertices[i].x, vertices[i].y);
  }
}

template<GLint D>
inline void GLBufferedGeometryD<D>::GetVertex(GLint i, vec3 &v) const
{
  if constexpr(D == 3) {
    v = vertices[i];
  } else {
    v = vec3(vertices[i].x, vertices[i].y, 0.0f);
  }
}

using GL2DBufferedGeometry = GLBufferedGeometryD<2>;
using GL3DBufferedGeometry = GLBufferedGeometryD<3>;
