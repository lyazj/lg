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

static constexpr const char *defaultPositioningSnippet = R"(
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

in vec3 a_position;
in vec3 a_normal;
out vec3 v_position;
out vec3 v_normal;

void position(void)
{
  v_position = (u_model * vec4(a_position, 1.0)).xyz;
  v_normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);
  gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
)";

GLShaderPtr GLShader::GetDefaultVertexShader()
{
  return make_shared<GLShader>(GL_VERTEX_SHADER,
      R"(
#version 150

in vec4 a_color;
out vec4 v_color;

void position(void);

void main()
{
  position();
  v_color = a_color;
}
)"s + defaultPositioningSnippet);
}

GLShaderPtr GLShader::GetDefaultTextureVertexShader()
{
  return make_shared<GLShader>(GL_VERTEX_SHADER,
      R"(
#version 150

in vec2 a_texCoord0;
out vec2 v_texCoord0;

void position(void);

void main()
{
  position();
  v_texCoord0 = a_texCoord0;
}
)"s + defaultPositioningSnippet);
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

static constexpr const char *defaultLightingSnippet = R"(
layout(std140) uniform u_light {
  vec4 position;  // w=0: parallel; w=1: point
  vec4 color;     // a discarded
  float distance;
  float ambient;
};

layout(std140) uniform u_highlight {
  vec4 viewPos;    // w discarded
  vec4 highColor;  // a discarded
  float shininess;
};

in vec3 v_position;
in vec3 v_normal;
out vec4 f_color;

void light(vec4 p_color)
{
  float d = length(position.xyz - v_position * position.w);
  vec3 l = normalize(position.xyz - v_position * position.w);
  vec3 n = normalize(v_normal);
  vec3 v = normalize(viewPos.xyz - v_position);
  vec3 h = normalize(l + v);
  float atten = clamp(1.0 - position.w * log(d / distance) / log(1.0e3), 0.0, 1.0);
  vec3 diffuse = color.rgb * max(dot(n, l), 0.0);
  vec3 specular = highColor.rgb * pow(max(dot(n, h), 0.0), shininess) * step(0.0, dot(n, l));
  f_color = vec4(atten * (p_color.rgb * (diffuse + ambient) + specular), p_color.a);
}
)";

GLShaderPtr GLShader::GetDefaultLightingFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER, R"(
#version 150

in vec4 v_color;

void light(vec4 p_color);

void main()
{
  light(v_color);
}
)"s + defaultLightingSnippet);
}

GLShaderPtr GLShader::GetLightingTextureFragmentShader()
{
  return make_shared<GLShader>(GL_FRAGMENT_SHADER, R"(
#version 150

uniform sampler2D u_texture0;

in vec2 v_texCoord0;

void light(vec4 p_color);

void main()
{
  light(texture(u_texture0, v_texCoord0));
}
)"s + defaultLightingSnippet);
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
  if(position.w == 0.0f) return;  // Parallel light doesn't attenuate.
  color /= (1.0f + ambient) * GetAttenuation(length(vec3(position) - p), distance);
}

void GLShader::DefaultLightingBlock::SetNearLight()
{
  position = vec4(0.0f, 2.0f, 2.0f, 1.0f);
  color = vec4(1.0f);
  distance = 1.0f;
  ambient = 0.3f;
  SetLightPoint({ 0.0f, 1.0f, 1.0f });
}

void GLShader::DefaultLightingBlock::SetMediumLight()
{
  position = vec4(0.0f, 4.0f, 4.0f, 1.0f);
  color = vec4(1.0f);
  distance = 1.0f;
  ambient = 0.3f;
  SetLightPoint({ 0.0f, 2.0f, 2.0f });
}

void GLShader::DefaultLightingBlock::SetFarLight()
{
  position = vec4(0.0f, 10.0f, 10.0f, 1.0f);
  color = vec4(1.0f);
  distance = 1.0f;
  ambient = 0.3f;
  SetLightPoint({ 0.0f, 5.0f, 5.0f });
}

void GLShader::DefaultHighlightBlock::Disable()
{
  viewPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);    // placeholder
  highColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);  // disabled
  shininess = 1.0f;                          // placeholder
}
