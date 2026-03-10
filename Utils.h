#pragma once

#include <utility>

#include "Global.h"

const char *GetProgramName();
const char *GetProgramShortName();
int GetElapsedTime();

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
