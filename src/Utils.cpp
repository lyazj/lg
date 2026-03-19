#include "Utils.h"

#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

static uint64_t gStartTime = GetTime();

uint64_t GetTime()
{
  using namespace std::chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

uint64_t GetSystemTime()
{
  using namespace std::chrono;
  return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

uint64_t GetStartTime() { return gStartTime; }

uint64_t GetElapsedTime() { return GetTime() - GetStartTime(); }

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

vec2 RandDirection2D()
{
  GLfloat phi = RandFloat() * 2.0f * pi;
  return { cosf(phi), sinf(phi) };
}

vec3 RandDirection3D()
{
  GLfloat z = RandFloat() * 2.0f - 1.0f;
  GLfloat phi = RandFloat() * 2.0f * pi;
  GLfloat rho = sqrtf(1.0f - z * z);
  return { rho * cosf(phi), rho * sinf(phi), z };
}

mat4 RandRotation2D()
{
  GLfloat phi = RandFloat() * 2.0f * pi;
  mat4 rotation(1.0f);
  rotation = rotate(rotation, phi, vec3(0.0f, 0.0f, 1.0f));
  return rotation;
}

mat4 RandRotation3D()
{
  GLfloat theta = acosf(RandFloat() * 2.0f - 1.0f);
  GLfloat phi = RandFloat() * 2.0f * pi;
  mat4 rotation(1.0f);
  rotation = rotate(rotation, theta, vec3(0.0f, 1.0f, 0.0f));
  rotation = rotate(rotation, phi, vec3(0.0f, 0.0f, 1.0f));
  return rotation;
}

std::ostream &operator<<(std::ostream &os, const vec2 &v)
{
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const vec3 &v)
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const vec4 &v)
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const mat2 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << "]]";
  return os;
}

std::ostream &operator<<(std::ostream &os, const mat3 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << "],\n"
     << " [" << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << "]]";
  return os;
}

std::ostream &operator<<(std::ostream &os, const mat4 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << "],\n"
     << " [" << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << "],\n"
     << " [" << m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3] << "]]";
  return os;
}

GLint SolveLinear(GLfloat x[1], const GLfloat a_in[2])
{
  GLfloat a = a_in[0], b = a_in[1];
  if(a == 0.0f) return 0;  // no solution or infinite solutions
  x[0] = -b / a;
  return 1;  // one solution
}

GLint SolveQuadratic(GLcomplex x[2], const GLfloat a_in[3], GLfloat *d_in)
{
  GLfloat a = a_in[0], b = a_in[1], c = a_in[2];
  if(a == 0.0f) {
    GLfloat x1;
    if(SolveLinear(&x1, &a_in[1]) == 0) return 0;
    x[0] = x1;
    return 1;  // one solution
  }
  b /= a, c /= a;
  GLfloat d = (b * 0.5f) * (b * 0.5f) - c;
  GLcomplex sd = sqrt((GLcomplex)d);
  x[0] = -b * 0.5f + sd;
  x[1] = -b * 0.5f - sd;
  if(d_in) *d_in = d;
  return 2;  // two solutions; might duplicate
}

GLint SolveBisection(GLfloat x[1], function<GLfloat(GLfloat)> f, GLfloat l, GLfloat r)
{
  GLfloat fl = f(l), fr = f(r);
  if(fl == 0.0f || fr == 0.0f) {
    x[0] = (fl == 0.0f) ? l : r;
    return (fl == 0.0f) + (fr == 0.0f);  // one solution or infinite solutions
  }
  if(fl * fr > 0.0f) return 0;  // no solution

  GLfloat l0, r0, m, fm;
  do {
    l0 = l, r0 = r, m = (l + r) * 0.5f, fm = f(m);
    if(fm == 0.0f) {
      x[0] = m;
      return 1;  // one solution
    }
    if(fm * fl < 0.0f) {
      r = m, fr = fm;
    } else {
      l = m, fl = fm;
    }
  } while(l != l0 || r != r0);
  x[0] = m;
  return 1;  // one solution
}

static GLcomplex cbrt(GLcomplex z)
{
  GLfloat r = abs(z);
  GLfloat theta = atan2f(z.imag(), z.real());
  return { cbrtf(r) * cosf(theta / 3.0f), cbrtf(r) * sinf(theta / 3.0f) };
}

GLint SolveCubic(GLcomplex x[3], const GLfloat a_in[4], GLfloat *d_in)
{
  GLfloat a = a_in[0], b = a_in[1], c = a_in[2], d = a_in[3];
  if(a == 0.0f) return SolveQuadratic(x, &a_in[1], d_in);
  b /= a, c /= a, d /= a;

  // x = u - b/3
  // u^3 + pu + q = 0
  GLfloat p = c - b * b / 3.0f, q = d + (2.0f * b * b * b - 9.0f * b * c) / 27.0f;

  // u = s + t, st = -p/3, s^3 + t^3 + q = 0
  // X^2 + qX - p^3/27 = 0, with X <- s^3, t^3
  GLcomplex st[2];
  GLfloat a_st[3] = { 1.0f, q, -p * p * p / 27.0f };
  SolveQuadratic(st, a_st, d_in);
  GLcomplex s = cbrt(st[0]), t = -p / (3.0f * s);

  GLcomplex w1 = { cosf(2.0f * pi / 3.0f), sinf(2.0f * pi / 3.0f) };
  GLcomplex w2 = { cosf(4.0f * pi / 3.0f), sinf(4.0f * pi / 3.0f) };
  x[0] = s + t - b / 3.0f;
  x[1] = s * w1 + t * w2 - b / 3.0f;
  x[2] = s * w2 + t * w1 - b / 3.0f;
  return 3;  // three solutions; might duplicate
}

fs::path GetResourcePath() { return fs::path("..") / "share"; }

fs::path GetTexturePath() { return GetResourcePath() / "textures"; }

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
