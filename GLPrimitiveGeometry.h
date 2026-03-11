#pragma once

#include "GLGeometry.h"

class GLBuffer;
class GLVertexArray;

class GLPrimitiveGeometry : public GLGeometry {
public:
  GLPrimitiveGeometry() = default;
  ~GLPrimitiveGeometry() override = default;

  virtual const GLVertexArray &GetVertexArray() const = 0;
  virtual const GLBuffer &GetVertexBuffer() const = 0;
  GLVertexArray &GetVertexArray() { return (GLVertexArray &)((const GLPrimitiveGeometry *)this)->GetVertexArray(); }
  GLBuffer &GetVertexBuffer() { return (GLBuffer &)((const GLPrimitiveGeometry *)this)->GetVertexBuffer(); }

  virtual GLint GetDimension() const = 0;
  virtual GLint GetNVertex() const = 0;
  virtual void GetVertex(GLint i, vec2 &v) const = 0;
  virtual void GetVertex(GLint i, vec3 &v) const = 0;
};
