#pragma once

#include "Global.h"

class GLImage;

class GLTexture {
public:
  GLTexture(GLenum t = GL_TEXTURE_2D);
  ~GLTexture();
  GLTexture(const GLTexture &) = delete;
  GLTexture &operator=(const GLTexture &) = delete;

  GLenum GetTarget() const { return target; }
  void SetTarget(GLenum t) { target = t; }

  void Bind() const;
  void Unbind() const;
  void Texture(GLenum iFormat, GLsizei width, GLenum format, const void *data) const;
  void Texture(GLenum iFormat, GLsizei width, GLsizei height, GLenum format, const void *data) const;
  void Texture(GLenum iFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, const void *data) const;
  void Texture(GLenum iFormat, const GLImage &image) const;
  void Texture(const GLImage &image) const;

private:
  GLenum target;
  GLuint id;
};
