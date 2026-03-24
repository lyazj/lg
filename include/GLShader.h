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
  static GLShaderPtr GetDefaultFragmentShader();
  static GLShaderPtr GetDefaultTextureFragmentShader();
  static GLShaderPtr GetDefaultLightingFragmentShader();

  struct DefaultLightingBlock {
    vec4 position;     // xyz = position
    vec4 color;        // rgb = color, a = intensity
    vec4 attenuation;  // x = constant, y = linear, z = quadratic

    void SetNearLight();
    void SetMediumLight();
    void SetFarLight();
  };

private:
  GLenum type;
  GLuint id;
};
