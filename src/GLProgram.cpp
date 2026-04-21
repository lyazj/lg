#include "GLProgram.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_map>

#include "GLApplication.h"
#include "GLShader.h"
#include "GLUniformBlock.h"
#include "Utils.h"

GL_DEFINE_WRAPPER(UniformBlockBinding)

using namespace std;

static const unordered_map<string, GLint> vertexAttributeMap{
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
  uniformBlockMap.clear();
  nBindingPoint = 0;
}

void GLProgram::Use() const
{
  glUseProgram(id);
  for(const auto &[uniform, block] : uniformBlockMap) {
    if(block) block->Bind();
  }
}

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

void GLProgram::DisableVertexAttribute(const char *name)
{
  GLint location = GetVertexAttributeLocation(name);
  glDisableVertexAttribArray(location);
}

void GLProgram::SetDefaultVertexAttributes()
{
  GLProgram::SetDefaultVertexAttribute("a_normal");
  GLProgram::SetDefaultVertexAttribute("a_color");
}

void GLProgram::SetDefaultVertexAttribute(const char *name)
{
  if(strcmp(name, "a_normal") == 0) {
    GLProgram::SetVertexAttribute("a_normal", vec3(0.0f, 0.0f, 1.0f));
  } else if(strcmp(name, "a_color") == 0) {
    GLProgram::SetVertexAttribute("a_color", vec4(0.0f, 0.0f, 1.0f, 1.0f));
  } else {
    cerr << "Warning: no default value for vertex attribute '" << name << "'" << endl;
  }
}

void GLProgram::SetVertexAttribute(const char *name, GLfloat value)
{
  GLint location = GetVertexAttributeLocation(name);
  glVertexAttrib1f(location, value);
}

void GLProgram::SetVertexAttribute(const char *name, GLint value)
{
  GLint location = GetVertexAttributeLocation(name);
  glVertexAttribI1i(location, value);
}

void GLProgram::SetVertexAttribute(const char *name, const vec2 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glVertexAttrib2fv(location, value_ptr(value));
}

void GLProgram::SetVertexAttribute(const char *name, const vec3 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glVertexAttrib3fv(location, value_ptr(value));
}

void GLProgram::SetVertexAttribute(const char *name, const vec4 &value)
{
  GLint location = GetVertexAttributeLocation(name);
  glVertexAttrib4fv(location, value_ptr(value));
}

GLint GLProgram::GetUniformLocation(const char *name) const
{
  auto [it, inserted] = uniformMap.emplace(name, -1);
  if(inserted) {
    it->second = glGetUniformLocation(id, name);
    //if(it->second < 0) cerr << "Debug: uniform '" << name << "' not found in program " << id << endl;
  }
  return it->second;
}

GLUniformBlock *GLProgram::GetUniformBlock(const char *name) const
{
  auto [it, inserted] = uniformBlockMap.emplace(name, nullptr);
  if(inserted) {
    GLuint index = glGetUniformBlockIndex(id, name);
    if(index != GL_INVALID_INDEX) {
      GLuint bindingPoint = nBindingPoint++;
      GLUniformBlockBinding(id, index, bindingPoint);
      it->second = make_unique<GLUniformBlock>(bindingPoint);
      if(GLApplication::GetInstance()->GetProgram().get() == this) it->second->Bind();
    }
  }
  return it->second.get();
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

void GLProgram::SetUniform(const char *name, const vec2 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform2fv(uniform, 1, value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const vec3 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform3fv(uniform, 1, value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const vec4 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniform4fv(uniform, 1, value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const mat3 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniformMatrix3fv(uniform, 1, GL_FALSE, value_ptr(value));
}

void GLProgram::SetUniform(const char *name, const mat4 &value) const
{
  GLint uniform = GetUniformLocation(name);
  if(uniform >= 0) glUniformMatrix4fv(uniform, 1, GL_FALSE, value_ptr(value));
}

void GLProgram::AssignDefaultTextureUnits() const
{
  // Assign sampler uniforms u_texture0 and u_texture1 to texture units 0 and 1.
  // GLProgramGuard ensures the program is active while setting uniforms.
  GLProgramPtr program((GLProgram *)this, [](GLProgram *) { });  // fake shared_ptr
  GLProgramGuard guard(program);
  SetUniform("u_texture0", 0);
  SetUniform("u_texture1", 1);
}

void GLProgram::SetUniformBlock(const char *name, GLsizeiptr size, const void *value) const
{
  GLUniformBlock *uniformBlock = GetUniformBlock(name);
  if(uniformBlock) uniformBlock->Buffer(size, value);
}

GLProgramPtr GLProgram::GetDefaultProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetDefaultVertexShader());
  program->AttachShader(GLShader::GetDefaultFragmentShader());
  program->Link();
  program->DetachShaders();
  return program;
}

GLProgramPtr GLProgram::GetDefaultTextureProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetDefaultTextureVertexShader());
  program->AttachShader(GLShader::GetDefaultTextureFragmentShader());
  program->Link();
  program->DetachShaders();
  program->AssignDefaultTextureUnits();
  return program;
}

static void InitLighting(const GLProgramPtr &program)
{
  GLShader::DefaultLightingBlock lighting;
  lighting.SetNearLight();
  program->SetUniformBlock("u_light", &lighting);
  program->ResetDefaultHighlight();
}

GLProgramPtr GLProgram::GetDefaultLightingProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetDefaultVertexShader());
  program->AttachShader(GLShader::GetDefaultLightingFragmentShader());
  program->Link();
  program->DetachShaders();
  InitLighting(program);
  return program;
}

GLProgramPtr GLProgram::GetLightingTextureProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetDefaultTextureVertexShader());
  program->AttachShader(GLShader::GetLightingTextureFragmentShader());
  program->Link();
  program->DetachShaders();
  InitLighting(program);
  program->AssignDefaultTextureUnits();
  return program;
}

GLProgramPtr GLProgram::GetFontTextureProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetFontTextureVertexShader());
  program->AttachShader(GLShader::GetFontTextureFragmentShader());
  program->Link();
  program->DetachShaders();
  program->AssignDefaultTextureUnits();
  return program;
}

void GLProgram::SetDefaultHighlight(const vec3 &highColor, GLfloat shininess, bool attenuating) const
{
  GLShader::DefaultHighlightBlock highlight;
  highlight.viewPos = GLApplication::GetInstance()->GetViewerPosition();
  highlight.hiColor = vec4(highColor, 1.0f);
  highlight.hiShine = shininess;
  highlight.hiAtten = attenuating;
  SetUniformBlock("u_highlight", &highlight);
}

void GLProgram::ResetDefaultHighlight() const
{
  GLShader::DefaultHighlightBlock highlight;
  highlight.Disable();
  SetUniformBlock("u_highlight", &highlight);
}

GLProgramPtr GLProgram::GetGouraudLightingProgram()
{
  auto program = make_shared<GLProgram>();
  program->AttachShader(GLShader::GetLightingVertexShader());
  program->AttachShader(GLShader::GetDefaultFragmentShader());
  program->Link();
  program->DetachShaders();
  InitLighting(program);
  return program;
}
