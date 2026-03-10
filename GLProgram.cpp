#include "GLProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_map>

#include "GLApplication.h"
#include "GLShader.h"
#include "Utils.h"

using namespace std;

const std::unordered_map<std::string, GLint> GLProgram::vertexAttributeMap{
  { "a_position", 0 },
  { "a_normal", 1 },
  { "a_tangent", 2 },
  { "a_bitangent", 3 },
  { "a_color", 4 },
  { "a_texCoord0", 5 },
  { "a_texCoord1", 6 },
};

GLProgram::GLProgram()
{
  id = glCreateProgram();
  if(id == 0) abort();
}

GLProgram::~GLProgram()
{
  DetachShaders();
  glDeleteProgram(id);
}

void GLProgram::AttachShader(const GLShaderPtr &shader)
{
  if(shaders.insert(shader).second) shader->Attach(id);
}

void GLProgram::DetachShader(const GLShaderPtr &shader)
{
  GLShaderPtr p = shader;
  if(shaders.erase(shader)) p->Detach(id);
}

void GLProgram::DetachShaders()
{
  for(const GLShaderPtr &shader : shaders) shader->Detach(id);
  shaders.clear();
}

void GLProgram::Link()
{
  for(const auto &[name, index] : vertexAttributeMap) glBindAttribLocation(id, index, name.c_str());
  GLLinkProgram(id);
  uniformMap.clear();
}

void GLProgram::Use() const { glUseProgram(id); }

GLProgram *GLProgram::GetCurrentProgram() { return GLApplication::GetInstance()->GetProgram().get(); }

GLint GLProgram::GetVertexAttributeLocation(const char *name) { return vertexAttributeMap.at(name); }

void GLProgram::SetVertexAttributePointer(
    const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
{
  GLint location = GetVertexAttributeLocation(name);
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, size, type, normalized, stride, pointer);
}

void GLProgram::SetVertexAttributePointer(const char *name, GLint size, const void *pointer)
{
  SetVertexAttributePointer(name, size, GL_FLOAT, GL_FALSE, 0, pointer);
}

void GLProgram::SetVertexAttribute(const char *name, GLfloat value)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
  glVertexAttrib1f(location, value);
}

void GLProgram::SetVertexAttribute(const char *name, GLint value)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
  glVertexAttribI1i(location, value);
}

void GLProgram::SetVertexAttribute(const char *name, const glm::vec2 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
  glVertexAttrib2fv(location, glm::value_ptr(value));
}

void GLProgram::SetVertexAttribute(const char *name, const glm::vec3 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
  glVertexAttrib3fv(location, glm::value_ptr(value));
}

void GLProgram::SetVertexAttribute(const char *name, const glm::vec4 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
  glVertexAttrib4fv(location, glm::value_ptr(value));
}

GLint GLProgram::GetUniformLocation(const char *name) const
{
  auto [it, inserted] = uniformMap.emplace(name, -1);
  if(inserted) {
    it->second = glGetUniformLocation(id, name);
    if(it->second < 0) cerr << "Warning: uniform '" << name << "' not found in program " << id << endl;
  }
  return it->second;
}

void GLProgram::SetUniform(const char *name, GLfloat value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform1f(uniform, value);
}

void GLProgram::SetUniform(const char *name, GLint value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform1i(uniform, value);
}

void GLProgram::SetUniform(const char *name, const glm::vec2 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform2fv(uniform, 1, glm::value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const glm::vec3 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform3fv(uniform, 1, glm::value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const glm::vec4 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform4fv(uniform, 1, glm::value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const glm::mat3 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniformMatrix3fv(uniform, 1, GL_FALSE, glm::value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const glm::mat4 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(value));
}

GLProgramPtr GLProgram::GetDefaultProgram()
{
  GLProgramPtr program(new GLProgram);
  program->AttachShader(GLShader::GetDefaultVertexShader());
  program->AttachShader(GLShader::GetDefaultFragmentShader());
  program->Link();
  program->DetachShaders();
  return program;
}
