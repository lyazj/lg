#pragma once

#include <iterator>

#include "Global.h"

class GLBuffer {
public:
  GLBuffer(GLenum t = GL_ARRAY_BUFFER, GLenum u = GL_STATIC_DRAW);
  ~GLBuffer();
  GLBuffer(const GLBuffer &) = delete;
  GLBuffer &operator=(const GLBuffer &) = delete;

  GLenum GetTarget() const { return target; }
  void SetTarget(GLenum t) { target = t; }
  GLenum GetUsage() const { return usage; }
  void SetUsage(GLenum u) { usage = u; }

  void Bind() const;
  void Unbind() const;
  void Buffer(GLsizeiptr size, const void *data) const;
  template<class T>
  void Buffer(const T &data) const;

private:
  GLenum target;
  GLenum usage;
  GLuint id;
};

template<class T>
void GLBuffer::Buffer(const T &data) const
{
  Buffer(std::size(data) * sizeof(*std::data(data)), std::data(data));
}
