#pragma once

#include "GLBuffer.h"

class GLUniformBlock {
public:
  GLUniformBlock(GLuint bindingPoint);
  ~GLUniformBlock();
  GLUniformBlock(const GLUniformBlock &) = delete;
  GLUniformBlock &operator=(const GLUniformBlock &) = delete;

  GLuint GetBindingPoint() const { return bindingPoint; }
  GLBuffer &GetBuffer() { return buffer; }
  const GLBuffer &GetBuffer() const { return buffer; }

  void Bind() const;
  void Unbind() const;
  void Buffer(GLsizeiptr size, const void *data) const { buffer.Buffer(size, data); }
  template<class T>
  void Buffer(const T *data) const;

private:
  GLuint bindingPoint;
  GLBuffer buffer;
};

template<class T>
inline void GLUniformBlock::Buffer(const T *data) const
{
  Buffer(sizeof(T), data);
}
