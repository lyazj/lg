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
