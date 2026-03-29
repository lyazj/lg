#include "Utils.h"

#include <locale.h>
#include <math.h>

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <random>
#include <vector>

#include "FileMap.h"

#ifdef _WIN32
#include <windows.h>
#else /* _WIN32 */
#include <signal.h>
#endif /* _WIN32 */

#ifdef HAS_ICU
#include <unicode/unistr.h>
#else /* HAS_ICU */
#include <codecvt>
#endif /* HAS_ICU */

using namespace std;

static uint64_t gStartTime = GetTime();

uint64_t GetTime()
{
  using namespace chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

uint64_t GetSystemTime()
{
  using namespace chrono;
  return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

uint64_t GetStartTime() { return gStartTime; }

uint64_t GetElapsedTime() { return GetTime() - GetStartTime(); }

void SetDefaultLocale() { clog << "Info: applying default locale: " << setlocale(LC_ALL, "") << endl; }

string ToLocaleString(const wstring &wstr)
{
  string result;
  result.reserve(2 * wstr.size());
  string buffer(MB_CUR_MAX, 0);
  for(wchar_t wc : wstr) {
    size_t len = wctomb(buffer.data(), wc);  // thread-unsafe
    if(len == (size_t)-1) {
      cerr << "Error: error converting wide character: " << (uint32_t)wc << endl;
      buffer[0] = '?', len = 1;
    }
    result.append(buffer.data(), len);
  }
  result.shrink_to_fit();
  return result;
}

wstring FromLocaleString(const string &str)
{
  wstring result;
  result.reserve(str.size());
  wchar_t buffer;
  const char *ptr = str.c_str();
  while(*ptr) {
    size_t len = mbtowc(&buffer, ptr, MB_CUR_MAX);  // thread-unsafe
    if(len == (size_t)-1) {
      cerr << "Error: error converting narrow character: " << (uint32_t)*ptr << endl;
      buffer = (wchar_t)'?', len = 1;
    }
    result.push_back(buffer);
    ptr += len;
  }
  result.shrink_to_fit();
  return result;
}

#ifdef HAS_ICU
namespace {

template<class String>
icu::UnicodeString FromString8(const String &str)
{
  return icu::UnicodeString::fromUTF8(icu::StringPiece((const char *)str.data(), (int32_t)str.length()));
}

template<class String>
icu::UnicodeString FromString16(const String &str)
{
  return icu::UnicodeString((const UChar *)str.c_str(), (int32_t)str.length());
}

template<class String>
icu::UnicodeString FromString32(const String &str)
{
  return icu::UnicodeString::fromUTF32((const UChar32 *)str.c_str(), (int32_t)str.length());
}

template<class String>
String ToString8(const icu::UnicodeString &ustr)
{
  String result;
  result.reserve(ustr.length() * 2);  // A factor of 2 is a good guess.
  ustr.toUTF8String(result);
  result.shrink_to_fit();
  return result;
}

template<class String>
String ToString16(const icu::UnicodeString &ustr)
{
  return String((const typename String::value_type *)ustr.getBuffer(), (size_t)ustr.length());
}

template<class String>
String ToString32(const icu::UnicodeString &ustr)
{
  String result(ustr.length(), 0);  // ustr.length() is a safe upper bound.
  UErrorCode errorCode = U_ZERO_ERROR;
  int32_t n = ustr.toUTF32((UChar32 *)result.data(), (int32_t)result.size(), errorCode);
  if(U_FAILURE(errorCode)) abort();
  result.resize((size_t)n);
  result.shrink_to_fit();
  return result;
}

}  // namespace
#else /* HAS_ICU */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif /* HAS_ICU */

string ToString(const u32string &u32str)
{
#ifdef HAS_ICU
  return ToString8<string>(FromString32(u32str));
#else  /* HAS_ICU */
  return wstring_convert<codecvt_utf8<char32_t>, char32_t>().to_bytes(u32str);
#endif /* HAS_ICU */
}

u32string FromString(const string &str)
{
#ifdef HAS_ICU
  return ToString32<u32string>(FromString8(str));
#else  /* HAS_ICU */
  return wstring_convert<codecvt_utf8<char32_t>, char32_t>().from_bytes(str);
#endif /* HAS_ICU */
}

static_assert(sizeof(wchar_t) == sizeof(char16_t) || sizeof(wchar_t) == sizeof(char32_t));

wstring ToWString(const u32string &u32str)
{
  if constexpr(sizeof(wchar_t) == sizeof(char32_t)) return wstring((const wchar_t *)u32str.data(), u32str.size());
#ifdef HAS_ICU
  return ToString16<wstring>(FromString32(u32str));
#else  /* HAS_ICU */
  return wstring_convert<codecvt_utf8<wchar_t>, wchar_t>().from_bytes(ToString(u32str));
#endif /* HAS_ICU */
}

u32string FromWString(const wstring &wstr)
{
  if constexpr(sizeof(wchar_t) == sizeof(char32_t)) return u32string((const char32_t *)wstr.data(), wstr.size());
#ifdef HAS_ICU
  return ToString32<u32string>(FromString16(wstr));
#else  /* HAS_ICU */
  return FromString(wstring_convert<codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wstr));
#endif /* HAS_ICU */
}

#ifndef HAS_ICU
#pragma GCC diagnostic pop
#endif /* HAS_ICU */

void ToFile(const fs::path &path, const u32string &s)
{
  string content = ToString(s);
  ofstream ofs(path, ios_base::binary);
  ofs.write(content.data(), content.size());
}

u32string FromFile(const fs::path &path)
{
  FileMap fileMap(path);
  string content(fileMap.data(), fileMap.size());
  return FromString(content);
}

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

ostream &operator<<(ostream &os, const vec2 &v)
{
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

ostream &operator<<(ostream &os, const vec3 &v)
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

ostream &operator<<(ostream &os, const vec4 &v)
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
  return os;
}

ostream &operator<<(ostream &os, const mat2 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << "]]";
  return os;
}

ostream &operator<<(ostream &os, const mat3 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << "],\n"
     << " [" << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << "]]";
  return os;
}

ostream &operator<<(ostream &os, const mat4 &m)
{
  os << "[[" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << "],\n"
     << " [" << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << "],\n"
     << " [" << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << "],\n"
     << " [" << m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3] << "]]";
  return os;
}

GLint SolveLinear(double x[1], const double a_in[2])
{
  double a = a_in[0], b = a_in[1];
  if(a == 0.0) return 0;  // no solution or infinite solutions
  x[0] = -b / a;
  return 1;  // one solution
}

GLint SolveQuadratic(complex<double> x[2], const double a_in[3], double d_in[1])
{
  double a = a_in[0], b = a_in[1], c = a_in[2];
  if(a == 0.0) {
    double x1;
    if(SolveLinear(&x1, &a_in[1]) == 0) return 0;
    x[0] = x1;
    return 1;  // one solution
  }
  b /= a, c /= a;
  double d = (b * 0.5) * (b * 0.5) - c;
  complex<double> sd = sqrt((complex<double>)d);
  x[0] = -b * 0.5 + sd;
  x[1] = -b * 0.5 - sd;
  if(d_in) d_in[0] = d;
  return 2;  // two solutions; might duplicate
}

GLint SolveCubic(complex<double> x[3], const double a_in[4], double d_in[1])
{
  double a = a_in[0], b = a_in[1], c = a_in[2], d = a_in[3];
  if(a == 0.0) return SolveQuadratic(x, &a_in[1], d_in);
  b /= a, c /= a, d /= a;

  // x = u - b/3
  // u^3 + pu + q = 0
  double p = c - b * b / 3.0, q = d + (2.0 * b * b * b - 9.0 * b * c) / 27.0;

  // u = s + t, st = -p/3, s^3 + t^3 + q = 0
  // X^2 + qX - p^3/27 = 0, with X <- s^3, t^3
  complex<double> st3[2], s, t;
  double a_st3[3] = { 1.0, q, -p * p * p / 27.0 };
  SolveQuadratic(st3, a_st3, d_in);
  if(st3[0].imag() == 0.0) {
    s = cbrt(st3[0].real()), t = cbrt(st3[1].real());
  } else {
    double s_abs = cbrt(abs(st3[0])), s_phi = atan2(st3[0].imag(), st3[0].real()) / 3.0;
    s = { s_abs * cos(s_phi), s_abs * sin(s_phi) }, t = conj(s);
  }

  complex<double> w1 = { cos(2.0 * M_PI / 3.0), sin(2.0 * M_PI / 3.0) };
  complex<double> w2 = { cos(4.0 * M_PI / 3.0), sin(4.0 * M_PI / 3.0) };
  x[0] = s + t - b / 3.0;
  x[1] = s * w1 + t * w2 - b / 3.0;
  x[2] = s * w2 + t * w1 - b / 3.0;
  return 3;  // three solutions; might duplicate
}

GLint SolveQuartic(complex<double> x[4], const double a_in[5], double d_in[1])
{
  double a = a_in[0], b = a_in[1], c = a_in[2], d = a_in[3], e = a_in[4];
  if(a == 0.0) return SolveCubic(x, &a_in[1], d_in);
  b /= a, c /= a, d /= a, e /= a;

  // x = y - b/4
  // y^4 + py^2 + qy + r
  double p = c - 3.0 * b * b / 8.0;
  double q = d + b * b * b / 8.0 - b * c / 2.0;
  double r = e - 3.0 * b * b * b * b / 256.0 + b * b * c / 16.0 - b * d / 4.0;

  if(q == 0.0) {  // y^4 + py^2 + r
    double a_y2[3] = { 1.0, p, r };
    complex<double> y2[2];
    SolveQuadratic(y2, a_y2, d_in);
    x[0] = sqrt(y2[0]) - b / 4.0;
    x[1] = -sqrt(y2[0]) - b / 4.0;
    x[2] = sqrt(y2[1]) - b / 4.0;
    x[3] = -sqrt(y2[1]) - b / 4.0;
    return -4;  // four solutions; biquadratic; might duplicate
  }

  // y^4 + py^2 + qy + r = (y^2 + sy + t)(y^2 - sy + u)
  // p = u - s^2 + t, q = s(u - t), r = tu, s != 0
  // u = (p + s^2 + q/s) / 2, t = (p + s^2 - q/s) / 2 -> r = tu
  // s^6 + 2ps^4 + (p^2 - 4r)s^2 - q^2 = 0
  double a_s2[4] = { 1.0, 2.0 * p, p * p - 4.0 * r, -q * q };
  complex<double> s2[3];
  SolveCubic(s2, a_s2);

  // y^2 + sy + t = 0 and y^2 - sy + u = 0
  double min_error = INFINITY;
  for(GLint i = 0; i < 3; ++i) {
    complex<double> s = sqrt(s2[i]);
    if(s == 0.0) continue;
    complex<double> t = (p + s * s - q / s) * 0.5, u = (p + s * s + q / s) * 0.5;
    complex<double> y[4];
    y[0] = -s * 0.5 + sqrt((s * 0.5) * (s * 0.5) - t);
    y[1] = -s * 0.5 - sqrt((s * 0.5) * (s * 0.5) - t);
    y[2] = +s * 0.5 + sqrt((s * 0.5) * (s * 0.5) - u);
    y[3] = +s * 0.5 - sqrt((s * 0.5) * (s * 0.5) - u);
    double error = 0.0;
    for(GLint j = 0; j < 4; ++j) error += abs(y[j] * y[j] * y[j] * y[j] + p * y[j] * y[j] + q * y[j] + r);
    if(error >= min_error) continue;
    min_error = error;
    for(GLint j = 0; j < 4; ++j) x[j] = y[j] - b / 4.0;
  }
  if(isinf(min_error)) return -1;  // falied
  return 4;                        // four solutions; might duplicate
}

GLint SolveBisection(double x[1], function<double(double)> f, double l, double r)
{
  double fl = f(l), fr = f(r);
  if(fl == 0.0 || fr == 0.0) {
    x[0] = (fl == 0.0) ? l : r;
    return (fl == 0.0) + (fr == 0.0);  // one solution or infinite solutions
  }
  if(fl * fr > 0.0) return 0;  // no solution

  double l0, r0, m, fm;
  do {
    l0 = l, r0 = r, m = (l + r) * 0.5, fm = f(m);
    if(fm == 0.0) {
      x[0] = m;
      return 1;  // one solution
    }
    if(fm * fl < 0.0) {
      r = m, fr = fm;
    } else {
      l = m, fl = fm;
    }
  } while(l != l0 || r != r0);
  x[0] = m;
  return 1;  // one solution
}

GLint SolveNewton(
    double x_in[1], function<double(double)> f, function<double(double)> fp, double xerr, double yerr, GLuint nit)
{
  double &x = x_in[0];
  while(nit--) {
    double fx = f(x), fpx = fp(x);
    if(fabs(fx) <= yerr) return 1;  // one solution
    double x0 = x;
    x = x - fx / fpx;
    if(fabs(x - x0) <= xerr) return 1;  // one solution
  }
  return -1;  // failed
}

fs::path GetResourcePath() { return fs::path("..") / "share"; }

fs::path GetTexturePath() { return GetResourcePath() / "textures"; }

fs::path GetFontPath() { return GetResourcePath() / "fonts"; }

fs::path GetTextPath() { return GetResourcePath() / "texts"; }

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

void Debug()
{
#ifdef _WIN32
  DebugBreak();
#else  /* _WIN32 */
  raise(SIGTRAP);
#endif /* _WIN32 */
}
