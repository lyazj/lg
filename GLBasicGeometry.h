#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLGeometry.h"
#include "GLVertexArray.h"

class GLBasicGeometryBase : public GLGeometry {
public:
  GLBasicGeometryBase();
  ~GLBasicGeometryBase() override;

  GLVertexArray &GetVertexArray() { return vertexArray; }
  const GLVertexArray &GetVertexArray() const { return vertexArray; }
  GLBuffer &GetVertexBuffer() { return vertexBuffer; }
  const GLBuffer &GetVertexBuffer() const { return vertexBuffer; }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLVertexArray vertexArray;
  GLBuffer vertexBuffer;

  virtual void Bind() const;
  virtual void SetUniforms(const mat4 &model) const;
  virtual void IssueBuffer() const = 0;
  virtual void IssueDraw() const = 0;

  virtual GLint GetDimension() const = 0;
  virtual GLint GetNVertex() const = 0;
  virtual void GetVertex(GLint i, vec2 &v) const = 0;
  virtual void GetVertex(GLint i, vec3 &v) const = 0;
};

template<GLint D>
class GLBasicGeometry : public GLBasicGeometryBase {
public:
  GLBasicGeometry() = default;
  ~GLBasicGeometry() override = default;

protected:
  std::vector<vec<D>> vertices;
  virtual void IssueBuffer() const override { vertexBuffer.Buffer(vertices); }

  virtual GLint GetDimension() const override final { return D; }
  virtual GLint GetNVertex() const override final { return (GLint)vertices.size(); }
  virtual void GetVertex(GLint i, vec2 &v) const override final;
  virtual void GetVertex(GLint i, vec3 &v) const override final;
  const vec<D> &GetVertex(GLint i) const { return vertices[i]; }
};

template<GLint D>
inline void GLBasicGeometry<D>::GetVertex(GLint i, vec2 &v) const
{
  if constexpr(D == 2) {
    v = vertices[i];
  } else {
    v = vec2(vertices[i].x, vertices[i].y);
  }
}

template<GLint D>
inline void GLBasicGeometry<D>::GetVertex(GLint i, vec3 &v) const
{
  if constexpr(D == 3) {
    v = vertices[i];
  } else {
    v = vec3(vertices[i].x, vertices[i].y, 0.0f);
  }
}
