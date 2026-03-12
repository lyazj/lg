#include "Utils.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

unsigned GetElapsedTime() { return (unsigned)glutGet(GLUT_ELAPSED_TIME); }

static mt19937 gRandom;

void RandSeed(unsigned long long s) { gRandom.seed((mt19937::result_type)s); }

GLint RandInt() { return uniform_int_distribution<GLint>()(gRandom); }

GLint RandInt(GLint a) { return RandInt(0, a); }

GLint RandInt(GLint a, GLint b)
{
  assert(a < b);
  return uniform_int_distribution<GLint>(a, b - 1)(gRandom);
}

GLfloat RandFloat() { return uniform_real_distribution<GLfloat>()(gRandom); }

vec2 RandVec2() { return { RandFloat(), RandFloat() }; }

vec3 RandVec3() { return { RandFloat(), RandFloat(), RandFloat() }; }

vec4 RandVec4() { return { RandFloat(), RandFloat(), RandFloat(), RandFloat() }; }

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
