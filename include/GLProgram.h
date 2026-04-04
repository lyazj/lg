#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Global.h"

GL_DECLARE_CLASS(GLUniformBlock)

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

  static GLint GetVertexAttributeLocation(const char *name);
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

  GLint GetUniformLocation(const char *name) const;         // thread-unsafe
  GLUniformBlock *GetUniformBlock(const char *name) const;  // thread-unsafe
  void SetUniform(const char *name, GLfloat value) const;   // The program must be used for SetUniform().
  void SetUniform(const char *name, GLint value) const;
  void SetUniform(const char *name, const vec2 &value) const;
  void SetUniform(const char *name, const vec3 &value) const;
  void SetUniform(const char *name, const vec4 &value) const;
  void SetUniform(const char *name, const mat3 &value) const;
  void SetUniform(const char *name, const mat4 &value) const;
  void AssignDefaultTextureUnits() const;  // Temporarily binds the program.
  void SetUniformBlock(const char *name, GLsizeiptr size, const void *value) const;
  template<class T>
  void SetUniformBlock(const char *name, const T *value) const;

  static GLProgramPtr GetDefaultProgram();
  static GLProgramPtr GetDefaultTextureProgram();
  static GLProgramPtr GetDefaultLightingProgram();
  static GLProgramPtr GetLightingTextureProgram();
  static GLProgramPtr GetFontTextureProgram();

private:
  GLuint id;
  std::unordered_set<GLShaderPtr> shaders;
  mutable std::unordered_map<std::string, GLint> uniformMap;
  mutable std::unordered_map<std::string, GLUniformBlockUPtr> uniformBlockMap;
  mutable GLint nBindingPoint;
};

template<class T>
inline void GLProgram::SetUniformBlock(const char *name, const T *value) const
{
  SetUniformBlock(name, sizeof(T), value);
}
