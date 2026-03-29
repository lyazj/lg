#pragma once

#include <functional>
#include <iosfwd>
#include <utility>

#include "Global.h"

uint64_t GetTime();
uint64_t GetSystemTime();
uint64_t GetStartTime();
uint64_t GetElapsedTime();
inline uint64_t NsToMs(uint64_t ns) { return (ns + 500'000) / 1'000'000; }

// Locale-dependent functions. Not thread-safe.
void SetDefaultLocale();
std::string ToLocaleString(const std::wstring &s);
inline std::string ToLocaleString(const fs::path &path) { return ToLocaleString(path.wstring()); }
std::wstring FromLocaleString(const std::string &s);

// Locale-independent functions.
std::string ToString(const std::u32string &s);
std::u32string FromString(const std::string &s);
std::wstring ToWString(const std::u32string &s);
std::u32string FromWString(const std::wstring &s);
void ToFile(const fs::path &path, const std::u32string &s);
std::u32string FromFile(const fs::path &path);

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

std::ostream &operator<<(std::ostream &os, const vec2 &v);
std::ostream &operator<<(std::ostream &os, const vec3 &v);
std::ostream &operator<<(std::ostream &os, const vec4 &v);
std::ostream &operator<<(std::ostream &os, const mat2 &m);
std::ostream &operator<<(std::ostream &os, const mat3 &m);
std::ostream &operator<<(std::ostream &os, const mat4 &m);

GLint SolveLinear(double x[1], const double a[2]);
GLint SolveQuadratic(complex<double> x[2], const double a[3], double d[1] = nullptr);
GLint SolveCubic(complex<double> x[3], const double a[4], double d[1] = nullptr);
GLint SolveQuartic(complex<double> x[4], const double a[5], double d[1] = nullptr);
GLint SolveBisection(double x[1], std::function<double(double)> f, double l, double r);
GLint SolveNewton(double x[1], std::function<double(double)> f, std::function<double(double)> fp, double xerr,
    double yerr, GLuint nit);

fs::path GetResourcePath();
fs::path GetTexturePath();
fs::path GetFontPath();
fs::path GetTextPath();

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

void Debug();
