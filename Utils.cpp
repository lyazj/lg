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
    cerr << "OpenGL error: " << error << endl;
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
