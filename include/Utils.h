#pragma once

#include <utility>

#include "Global.h"

uint64_t GetTime();
uint64_t GetSystemTime();
uint64_t GetStartTime();
uint64_t GetElapsedTime();
inline uint64_t NsToMs(uint64_t ns) { return (ns + 500'000) / 1'000'000; }

// Thread-unsafe.
void RandSeed(unsigned long long);
GLint RandInt();
GLint RandInt(GLint a);
GLint RandInt(GLint a, GLint b);
GLfloat RandFloat();
vec2 RandVec2();
vec3 RandVec3();
vec4 RandVec4();
vec2 RandDirection2D();
vec3 RandDirection3D();
mat4 RandRotation2D();
mat4 RandRotation3D();

GLint SolveLinear(GLfloat x[1], const GLfloat a[2]);
GLint SolveQuadratic(GLcomplex x[2], const GLfloat a[3], GLfloat *d = nullptr);
GLint SolveCubic(GLcomplex x[3], const GLfloat a[4], GLfloat *d = nullptr);

fs::path GetResourcePath();
fs::path GetTexturePath();

#ifdef __GNUC__
#define Expect(e, v) __builtin_expect((e), (v))
#else /* __GNUC__ */
#define Expect(e, v) (e)
#endif /* __GNUC__ */

void GLCheckError();

void GLCompileShader(GLuint shader);
void GLLinkProgram(GLuint program);

#define GL_DEFINE_WRAPPER(func)            \
  template<class... Args>                  \
  inline void GL##func(Args &&...args)     \
  {                                        \
    gl##func(std::forward<Args>(args)...); \
    GLCheckError();                        \
  }

GL_DEFINE_WRAPPER(DrawArrays)
GL_DEFINE_WRAPPER(DrawElements)
