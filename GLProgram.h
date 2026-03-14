#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Global.h"

class GLProgram {
public:
  GLProgram();
  ~GLProgram();
  GLProgram(const GLProgram &) = delete;
  GLProgram &operator=(const GLProgram &) = delete;

  void AttachShader(const GLShaderPtr &);
  void DetachShader(const GLShaderPtr &);
  void DetachShaders();
  void Link();
  void Use() const;  // Should only be called by GLApplication.

  static GLProgram *GetCurrentProgram();

  static GLint GetVertexAttributeLocation(const char *name);  // thread-unsafe
  static void SetVertexAttributePointer(const char *, GLint, GLenum, GLboolean, GLsizei, const void *);
  static void SetVertexAttributePointer(const char *name, GLint size, const void *pointer = nullptr);
  static void DisableVertexAttribute(const char *name);
  static void SetDefaultVertexAttributes();
  static void SetDefaultVertexAttribute(const char *name);
  static void SetVertexAttribute(const char *name, GLfloat value);
  static void SetVertexAttribute(const char *name, GLint value);
  static void SetVertexAttribute(const char *name, const vec2 &value);
  static void SetVertexAttribute(const char *name, const vec3 &value);
  static void SetVertexAttribute(const char *name, const vec4 &value);

  GLint GetUniformLocation(const char *name) const;
  void SetUniform(const char *name, GLfloat value) const;
  void SetUniform(const char *name, GLint value) const;
  void SetUniform(const char *name, const vec2 &value) const;
  void SetUniform(const char *name, const vec3 &value) const;
  void SetUniform(const char *name, const vec4 &value) const;
  void SetUniform(const char *name, const mat3 &value) const;
  void SetUniform(const char *name, const mat4 &value) const;

  static GLProgramPtr GetDefaultProgram();
  static GLProgramPtr GetDefaultTextureProgram();

private:
  GLuint id;
  std::unordered_set<GLShaderPtr> shaders;
  static const std::unordered_map<std::string, GLint> vertexAttributeMap;
  mutable std::unordered_map<std::string, GLint> uniformMap;
};
