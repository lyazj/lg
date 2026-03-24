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
  vec4 position;      // xyz = position
  vec4 color;         // rgb = color, a = intensity
  vec4 attenuation;   // x = constant, y = linear, z = quadratic
};

in vec3 v_position;
in vec3 v_normal;
out vec4 f_color;

void main()
{
  vec3 pos = position.xyz - v_position;
  float dist = length(pos);
  vec3 dir = normalize(pos);
  float atten = 1.0 / (attenuation.x + attenuation.y * dist + attenuation.z * (dist * dist));
  vec3 diffuse = color.rgb * color.a * max(dot(normalize(v_normal), dir), 0.0);
  f_color = vec4(atten * diffuse, 1.0);
}
  )");
}

// Parameters suggested by ChatGPT.
void GLShader::DefaultLightingBlock::SetNearLight()
{
  position = vec4(0.0f, 2.0f, 2.0f, 1.0f);
  color = vec4(1.0f, 0.95f, 0.85f, 1.15f);
  attenuation = vec4(1.0f, 0.60f, 0.45f, 0.0f);
}

// Parameters suggested by ChatGPT.
void GLShader::DefaultLightingBlock::SetMediumLight()
{
  position = vec4(0.0f, 4.0f, 4.0f, 1.0f);
  color = vec4(1.0f, 1.0f, 0.98f, 1.0f);
  attenuation = vec4(1.0f, 0.14f, 0.07f, 0.0f);
}

// Parameters suggested by ChatGPT.
void GLShader::DefaultLightingBlock::SetFarLight()
{
  position = vec4(0.0f, 10.0f, 10.0f, 1.0f);
  color = vec4(0.80f, 0.88f, 1.00f, 0.75f);
  attenuation = vec4(1.0f, 0.045f, 0.0075f, 0.0f);
}
