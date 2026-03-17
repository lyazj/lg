#pragma once

#include "Global.h"

class GLRenderable {
public:
  GLRenderable();
  virtual ~GLRenderable();
  GLRenderable(const GLRenderable &) = delete;
  GLRenderable &operator=(const GLRenderable &) = delete;

  virtual void SetVertexAttributes() const = 0;
  virtual void Buffer() const = 0;
  virtual void Draw(const mat4 &model) const = 0;
};
