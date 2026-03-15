#include "GLTexture.h"

#include "GLImage.h"
#include "Utils.h"

GL_DEFINE_WRAPPER(TexImage1D)
GL_DEFINE_WRAPPER(TexImage2D)
GL_DEFINE_WRAPPER(TexImage3D)

using namespace std;

GLTexture::GLTexture(GLenum t) : target(t)
{
  glGenTextures(1, &id);
  if(id == 0) abort();
}

GLTexture::~GLTexture() { glDeleteTextures(1, &id); }

void GLTexture::Bind() const { glBindTexture(target, id); }

void GLTexture::Unbind() const { glBindTexture(target, 0); }

void GLTexture::Texture(GLenum iFormat, GLsizei width, GLenum format, const void *data) const
{
  // [TODO]
  Bind();
  GLTexImage1D(target, 0, iFormat, width, 0, format, GL_UNSIGNED_BYTE, data);
}

void GLTexture::Texture(GLenum iFormat, GLsizei width, GLsizei height, GLenum format, const void *data) const
{
  Bind();
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLTexImage2D(target, 0, iFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(target);
}

void GLTexture::Texture(
    GLenum iFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, const void *data) const
{
  // [TODO]
  Bind();
  GLTexImage3D(target, 0, iFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
}

static GLenum GetImageFormat(const GLImage &image)
{
  switch(image.GetType()) {
  case GLImageType::RGB: return GL_RGB;
  case GLImageType::RGBA: return GL_RGBA;
  default: abort();
  }
}

void GLTexture::Texture(GLenum iFormat, const GLImage &image) const
{
  Texture(iFormat, image.GetWidth(), image.GetHeight(), GetImageFormat(image), image.GetData().data());
}

void GLTexture::Texture(const GLImage &image) const { Texture(GetImageFormat(image), image); }
