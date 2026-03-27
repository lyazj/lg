#include "GLShader.h"

#include <algorithm>
#include <glm/geometric.hpp>

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
out vec3 v_position;
out vec3 v_normal;
out vec4 v_color;

void main()
{
  v_position = (u_model * vec4(a_position, 1.0)).xyz;
  v_normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);
  v_color = a_color;
  gl_Position = u_projection * u_view * vec4(v_position, 1.0);
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
out vec3 v_position;
out vec3 v_normal;
out vec2 v_texCoord0;

void main()
{
  v_position = (u_model * vec4(a_position, 1.0)).xyz;
  v_normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);
  v_texCoord0 = a_texCoord0;
  gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
  )");
}

GLShaderPtr GLShader::GetFontTextureVertexShader()
{
  return make_shared<GLShader>(GL_VERTEX_SHADER,
      R"(
#version 150

uniform mat4 u_model;
uniform float u_winWidth;
uniform float u_winHeight;
uniform float u_depth;

in vec2 a_position;
in vec2 a_texCoord0;
out vec2 v_texCoord0;

void main()
{
  gl_Position = u_model * vec4(a_position, 0.0, 1.0);
  gl_Position = vec4(gl_Position.x / u_winWidth * 2.0 - 1.0, 1.0 - gl_Position.y / u_winHeight * 2.0, u_depth, 1.0);
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

GLShaderPtr GLShader::GetDefaultLightingFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER,
      R"(
#version 150

layout(std140) uniform u_light {
  vec3 position;
  vec3 color;
  float distance;
  float ambient;
};

in vec3 v_position;
in vec3 v_normal;
in vec4 v_color;
out vec4 f_color;

void main()
{
  vec3 pos = position - v_position;
  float dist = length(pos);
  vec3 dir = normalize(pos);
  float atten = clamp(1.0 - log(dist / distance) / log(1.0e3), 0.0, 1.0);
  vec3 diffuse = color.rgb * max(dot(normalize(v_normal), dir), 0.0);
  f_color = vec4(v_color.rgb * atten * ((diffuse + ambient) / (1.0 + ambient)), v_color.a);
}
  )");
}

GLShaderPtr GLShader::GetLightingTextureFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER,
      R"(
#version 150

uniform sampler2D u_texture0;

layout(std140) uniform u_light {
  vec3 position;
  vec3 color;
  float distance;
  float ambient;
};

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texCoord0;
out vec4 f_color;

void main()
{
  vec3 pos = position - v_position;
  float dist = length(pos);
  vec3 dir = normalize(pos);
  float atten = clamp(1.0 - log(dist / distance) / log(1.0e3), 0.0, 1.0);
  vec3 diffuse = color.rgb * max(dot(normalize(v_normal), dir), 0.0);
  f_color = texture(u_texture0, v_texCoord0);
  f_color = vec4(f_color.rgb * atten * ((diffuse + ambient) / (1.0 + ambient)), f_color.a);
}
  )");
}

GLShaderPtr GLShader::GetFontTextureFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER,
      R"(
#version 150

uniform sampler2D u_texture0;
uniform vec4 u_color;

in vec2 v_texCoord0;
out vec4 f_color;

void main()
{
  f_color = texture(u_texture0, v_texCoord0);
  f_color = f_color.r * u_color;
}
  )");
}

static GLfloat GetAttenuation(GLfloat dist, GLfloat distance)
{
  return clamp(1.0f - logf(dist / distance) / logf(1.0e3f), 0.0f, 1.0f);
}

void GLShader::DefaultLightingBlock::SetLightPoint(const vec3 &p)
{
  if(length(color) == 0.0f) return;
  color *= 1.0f / GetAttenuation(length(position - p), distance);
}

void GLShader::DefaultLightingBlock::SetNearLight()
{
  position = vec3(0.0f, 2.0f, 2.0f);
  color = vec3(1.0f);
  distance = 1.0f;
  ambient = 0.15f;
  SetLightPoint({ 0.0f, 1.0f, 1.0f });
}

void GLShader::DefaultLightingBlock::SetMediumLight()
{
  position = vec3(0.0f, 4.0f, 4.0f);
  color = vec3(1.0f);
  distance = 1.0f;
  ambient = 0.15f;
  SetLightPoint({ 0.0f, 2.0f, 2.0f });
}

void GLShader::DefaultLightingBlock::SetFarLight()
{
  position = vec3(0.0f, 10.0f, 10.0f);
  color = vec3(1.0f);
  distance = 1.0f;
  ambient = 0.15f;
  SetLightPoint({ 0.0f, 5.0f, 5.0f });
}
