#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <stdint.h>

#include <complex>
#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

using std::complex;

constexpr GLfloat pi = glm::pi<GLfloat>();
constexpr GLdouble pid = glm::pi<GLdouble>();
constexpr GLfloat rad = 1.0f;
constexpr GLdouble radd = 1.0;
constexpr GLfloat deg = pi / 180.0f;
constexpr GLdouble degd = pid / 180.0;

template<GLint D>
using vec = glm::vec<D, GLfloat, glm::defaultp>;
using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

template<GLint D>
using vecd = glm::vec<D, GLdouble, glm::defaultp>;
using vec2d = vecd<2>;
using vec3d = vecd<3>;
using vec4d = vecd<4>;

template<GLint D1, GLint D2 = D1>
using mat = glm::mat<D1, D2, GLfloat, glm::defaultp>;
using mat2 = mat<2>;
using mat3 = mat<3>;
using mat4 = mat<4>;

template<GLint D1, GLint D2 = D1>
using matd = glm::mat<D1, D2, GLdouble, glm::defaultp>;
using mat2d = matd<2>;
using mat3d = matd<3>;
using mat4d = matd<4>;

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
using std::byte;

std::ostream &operator<<(std::ostream &, const std::wstring &);
std::ostream &operator<<(std::ostream &, const fs::path &);
std::ostream &operator<<(std::ostream &, const std::u32string &);
inline std::ostream &operator<<(std::ostream &os, wchar_t c) { return os << std::wstring(1, c); }
inline std::ostream &operator<<(std::ostream &os, char32_t c) { return os << std::u32string(1, c); }

#define GL_DECLARE_CLASS(name)             \
  class name;                              \
  using name##Ptr = std::shared_ptr<name>; \
  using name##WPtr = std::weak_ptr<name>;  \
  using name##UPtr = std::unique_ptr<name>;

GL_DECLARE_CLASS(GLApplication)
GL_DECLARE_CLASS(GLProgram)
GL_DECLARE_CLASS(GLShader)
GL_DECLARE_CLASS(GLTexture)
GL_DECLARE_CLASS(GLRenderable)
GL_DECLARE_CLASS(GLSimpleRenderable)
GL_DECLARE_CLASS(GLBufferedGeometry)
GL_DECLARE_CLASS(GLCompositeRenderable)
GL_DECLARE_CLASS(GLTransformedRenderable)
