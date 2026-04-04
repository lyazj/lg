#pragma once

#include "Global.h"

class GLRenderable {
public:
  GLRenderable();
  virtual ~GLRenderable();
  GLRenderable(const GLRenderable &) = delete;
  GLRenderable &operator=(const GLRenderable &) = delete;

  void SetVertexAttributes(bool force);
  void Buffer(bool force);
  virtual void Draw(const mat4 &model) const = 0;

protected:
  virtual void IssueSetVertexAttributes(bool force) const = 0;
  virtual void IssueBuffer(bool force) const = 0;

private:
  bool vertexAttributesSet;
  bool buffered;
};
