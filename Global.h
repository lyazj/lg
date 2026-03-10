#pragma once

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/fwd.hpp>
#include <glm/gtc/constants.hpp>

constexpr GLfloat pi = glm::pi<GLfloat>();
constexpr GLfloat rad = 1.0f;
constexpr GLfloat deg = pi / 180.0f;

#include <memory>

namespace std { namespace filesystem {

}}  // namespace std::filesystem

namespace fs = std::filesystem;
using std::byte;

#define GL_DECLARE_CLASS(name) \
  class name;                  \
  using name##Ptr = std::shared_ptr<name>;

GL_DECLARE_CLASS(GLApplication)
GL_DECLARE_CLASS(GLProgram)
GL_DECLARE_CLASS(GLShader)
GL_DECLARE_CLASS(GLGeometry)
