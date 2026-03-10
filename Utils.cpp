#include "Utils.h"

#include <errno.h>

#include <iostream>
#include <vector>

using namespace std;

const char *GetProgramName() { return program_invocation_name; }

const char *GetProgramShortName() { return program_invocation_short_name; }

int GetElapsedTime() { return glutGet(GLUT_ELAPSED_TIME); }

void GLCheckError()
{
  GLenum error = glGetError();
  if(error == GL_NO_ERROR) return;
  do {
    const char *msg;
    switch(error) {
    case GL_INVALID_ENUM: msg = "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE: msg = "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: msg = "GL_INVALID_OPERATION"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
    case GL_OUT_OF_MEMORY: msg = "GL_OUT_OF_MEMORY"; break;
    default: msg = "unknown"; break;
    }
    cerr << "OpenGL error: " << msg << " (0x" << hex << error << dec << ")" << endl;
  } while((error = glGetError()) != GL_NO_ERROR);
  exit(EXIT_FAILURE);
}

void GLCompileShader(GLuint shader)
{
  glCompileShader(shader);
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(!compiled) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    vector<char> log(logLength);
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());
    cerr << log.data();
    exit(EXIT_FAILURE);
  }
}

void GLLinkProgram(GLuint program)
{
  glLinkProgram(program);
  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if(!linked) {
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    vector<char> log(logLength);
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    cerr << log.data();
    exit(EXIT_FAILURE);
  }
}
