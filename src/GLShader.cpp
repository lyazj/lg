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

in vec3 a_position;
in vec3 a_normal;
in vec4 a_color;
out vec3 v_normal;
out vec4 v_color;

void main()
{
  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
  v_normal = transpose(inverse(mat3(u_model))) * a_normal;
  v_color = a_color;
}
  )");
}

GLShaderPtr GLShader::GetDefaultTextureVertexShader()
{
  return make_shared<GLShader>(GL_VERTEX_SHADER,
      R"(
#version 150

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord0;
out vec3 v_normal;
out vec2 v_texCoord0;

void main()
{
  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
  v_normal = transpose(inverse(mat3(u_model))) * a_normal;
  v_texCoord0 = a_texCoord0;
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

GLShaderPtr GLShader::GetDefaultTextureFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER,
      R"(
#version 150

uniform sampler2D u_texture0;

in vec2 v_texCoord0;
out vec4 f_color;

void main()
{
  f_color = texture(u_texture0, v_texCoord0);
}
  )");
}
