#pragma once

#include "Global.h"

class GLShader {
public:
  GLShader(GLenum type, const char *source);
  ~GLShader();
  GLShader(const GLShader &) = delete;
  GLShader &operator=(const GLShader &) = delete;

  GLenum GetType() const { return type; }

  void Attach(GLuint program) const;
  void Detach(GLuint program) const;

  static GLShaderPtr GetDefaultVertexShader();
  static GLShaderPtr GetDefaultFragmentShader();

private:
  GLenum type;
  GLuint id;
};
