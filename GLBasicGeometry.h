#pragma once

#include "GLBuffer.h"
#include "GLGeometry.h"
#include "GLVertexArray.h"

class GLBasicGeometry : public GLGeometry {
public:
  GLBasicGeometry(GLint dimension);
  ~GLBasicGeometry() override;
  GLBasicGeometry(const GLBasicGeometry &) = delete;
  GLBasicGeometry &operator=(const GLBasicGeometry &) = delete;

  GLVertexArray &GetVertexArray() { return vertexArray; }
  const GLVertexArray &GetVertexArray() const { return vertexArray; }
  GLBuffer &GetVertexBuffer() { return vertexBuffer; }
  const GLBuffer &GetVertexBuffer() const { return vertexBuffer; }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const glm::mat4 &model) const override;

protected:
  GLVertexArray vertexArray;
  GLBuffer vertexBuffer;
  GLint dimension;

  virtual void Bind() const;
  virtual void SetUniforms(const glm::mat4 &model) const;
  virtual void IssueBuffer() const = 0;
  virtual void IssueDraw() const = 0;
};
