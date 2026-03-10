#include "GLShader.h"

#include "Utils.h"

using namespace std;

GLShader::GLShader(GLenum t, const char *s) : type(t)
{
  id = glCreateShader(type);
  if(id == 0) abort();
  glShaderSource(id, 1, &s, nullptr);
  GLCompileShader(id);
}

GLShader::~GLShader() { glDeleteShader(id); }

void GLShader::Attach(GLuint program) const { glAttachShader(program, id); }
void GLShader::Detach(GLuint program) const { glDetachShader(program, id); }

GLShaderPtr GLShader::GetDefaultVertexShader()
{
  return make_shared<GLShader>(GL_VERTEX_SHADER,
      R"(
#version 150

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

in vec4 a_position;
in vec4 a_color;
out vec4 v_color;

void main()
{
  gl_Position = u_projection * u_view * u_model * a_position;
  v_color = a_color;
}
  )");
}

GLShaderPtr GLShader::GetDefaultFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER,
      R"(
#version 150

in vec4 v_color;
out vec4 f_color;

void main()
{
  f_color = v_color;
}
  )");
}
