#pragma once

#include "Global.h"

class GLVertexArray {
public:
  GLVertexArray();
  ~GLVertexArray();
  GLVertexArray(const GLVertexArray &) = delete;
  GLVertexArray &operator=(const GLVertexArray &) = delete;

  void Bind() const;
  void Unbind() const;

private:
  GLuint id;
};
