#pragma once

#include "Global.h"

class GLGeometry {
public:
  GLGeometry();
  virtual ~GLGeometry();
  GLGeometry(const GLGeometry &) = delete;
  GLGeometry &operator=(const GLGeometry &) = delete;

  virtual void SetVertexAttributes() const = 0;
  virtual void Buffer() const = 0;
  virtual void Draw(const glm::mat4 &model) const = 0;
  void Draw() const;
};
