#pragma once

#include <GL/glew.h>

#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>

constexpr GLfloat pi = glm::pi<GLfloat>();
constexpr GLfloat rad = 1.0f;
constexpr GLfloat deg = pi / 180.0f;

template<GLint D>
using vec = glm::vec<D, GLfloat, glm::defaultp>;
using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

template<GLint D1, GLint D2 = D1>
using mat = glm::mat<D1, D2, GLfloat, glm::defaultp>;
using mat2 = mat<2>;
using mat3 = mat<3>;
using mat4 = mat<4>;

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
using std::byte;

#define GL_DECLARE_CLASS(name) \
  class name;                  \
  using name##Ptr = std::shared_ptr<name>;

GL_DECLARE_CLASS(GLApplication)
GL_DECLARE_CLASS(GLProgram)
GL_DECLARE_CLASS(GLShader)
GL_DECLARE_CLASS(GLRenderable)
GL_DECLARE_CLASS(GLSimpleRenderable)
GL_DECLARE_CLASS(GLBufferedGeometry)
GL_DECLARE_CLASS(GLCompositeRenderable)
GL_DECLARE_CLASS(GLTransformedRenderable)
