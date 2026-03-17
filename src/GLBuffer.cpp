#include "GLBuffer.h"

#include "Utils.h"

GL_DEFINE_WRAPPER(BufferData)

using namespace std;

GLBuffer::GLBuffer(GLenum t, GLenum u) : target(t), usage(u)
{
  glGenBuffers(1, &id);
  if(id == 0) abort();
}

GLBuffer::~GLBuffer() { glDeleteBuffers(1, &id); }

void GLBuffer::Bind() const { glBindBuffer(target, id); }

void GLBuffer::Unbind() const { glBindBuffer(target, 0); }

void GLBuffer::Buffer(GLsizeiptr size, const void *data) const
{
  Bind();
  GLBufferData(target, size, data, usage);
}
