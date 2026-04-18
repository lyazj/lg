#pragma once

#include <glm/vec3.hpp>

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
    vec3 position;
    GLfloat padding_0 = 1.0f;
    vec3 color;
    GLfloat padding_1 = 1.0f;
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
    vec3 viewPos;
    GLfloat padding_0 = 1.0f;
    vec3 highColor;
    GLfloat padding_1 = 1.0f;
    GLfloat shininess;
    GLfloat padding_2[3] = { 0.0f, 0.0f, 0.0f };

    void Disable();
  };
  static_assert(sizeof(DefaultHighlightBlock) == 48);

private:
  GLenum type;
  GLuint id;
};
