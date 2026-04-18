#pragma once

#include <glm/vec4.hpp>

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
  static GLShaderPtr GetDefaultTextureVertexShader();
  static GLShaderPtr GetFontTextureVertexShader();
  static GLShaderPtr GetDefaultFragmentShader();
  static GLShaderPtr GetDefaultTextureFragmentShader();
  static GLShaderPtr GetDefaultLightingFragmentShader();
  static GLShaderPtr GetLightingTextureFragmentShader();
  static GLShaderPtr GetFontTextureFragmentShader();

  struct DefaultLightingBlock {
    vec4 position;
    vec4 color;
    GLfloat distance;
    GLfloat ambient;
    GLfloat padding_2[2] = { 0.0f, 0.0f };

    void SetLightPoint(const vec3 &p);
    void SetNearLight();
    void SetMediumLight();
    void SetFarLight();
  };
  static_assert(sizeof(DefaultLightingBlock) == 48);

  struct DefaultHighlightBlock {
    vec4 viewPos;
    vec4 highColor;
    GLfloat shininess;
    GLfloat padding_2[3] = { 0.0f, 0.0f, 0.0f };

    void Disable();
  };
  static_assert(sizeof(DefaultHighlightBlock) == 48);

private:
  GLenum type;
  GLuint id;
};
