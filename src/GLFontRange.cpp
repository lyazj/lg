#include "GLFontRange.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "FileMap.h"
#include "GLApplication.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "GLTransformedRenderable.h"

#ifdef HAS_STB
#ifdef NEED_STB_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif /* NEED_STB_IMPLEMENTATION */
#include <stb_truetype.h>
#endif /* HAS_STB */

using namespace std;

GLProgramPtr GLFontRange::program;

namespace {

class GLFontRangeTextureDecorator : public GLBufferedTextureDecorator {
public:
  GLFontRangeTextureDecorator(GLSimpleRenderablePtr r, GLProgramPtr p, GLTexturePtr t)
      : GLBufferedTextureDecorator(std::move(r), std::move(p), std::move(t))
  {
    // empty
  }
  ~GLFontRangeTextureDecorator() override = default;

  void SetTexCoords(GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1)
  {
    texCoords = { { s0, t1 }, { s0, t0 }, { s1, t1 }, { s1, t0 } };
  }

protected:
  virtual void SetTexCoords() const override { }
};

}  // namespace

#ifdef HAS_STB
class GLFontRange::Inner {
public:
  Inner(size_t nChar) : charData(new stbtt_bakedchar[nChar]) { }
  ~Inner() { delete[] charData; }
  Inner(const Inner &) = delete;
  Inner &operator=(const Inner &) = delete;

  stbtt_bakedchar *CharData() { return charData; }
  const stbtt_bakedchar *CharData() const { return charData; }

private:
  stbtt_bakedchar *charData;
};
#else  /* HAS_STB */
class GLFontRange::Inner {
public:
  Inner(size_t) { }
  ~Inner() { }
  Inner(const Inner &) = delete;
  Inner &operator=(const Inner &) = delete;
};
#endif /* HAS_STB */

GLFontRange::GLFontRange(const FileMap &fMap, GLfloat fontH, GLint atlasW, GLint atlasH, wchar_t firstC, wchar_t lastC)
    : fontHeight(fontH), atlasWidth(atlasW), atlasHeight(atlasH), firstChar(firstC), lastChar(lastC)
{
  inner = new Inner(lastChar - firstChar + 1);
  vector<byte> data(atlasW * atlasH);

#ifdef HAS_STB
  stbtt_BakeFontBitmap((const unsigned char *)fMap.data(), 0, fontHeight, (unsigned char *)data.data(), atlasWidth,
      atlasHeight, firstChar, lastChar - firstChar + 1, inner->CharData());
#else  /* HAS_STB */
  cerr << "Warning: GLFontRange: STB not available, font loading not supported" << endl;
#endif /* HAS_STB */

  texture = make_shared<GLTexture>();
  texture->Texture(GL_RED, atlasWidth, atlasHeight, GL_RED, data.data());
}

GLFontRange::~GLFontRange() { delete inner; }

GLRenderablePtr GLFontRange::GetRenderable(wchar_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const
{
  if(c == '\n') {  // new line
    x = xmin, y += fontHeight;
    return nullptr;
  }
#ifdef HAS_STB
  if(c < firstChar || c > lastChar) {
    cerr << "Warning: GLFontRange: character " << (int)c << " out of range" << endl;
    c = '?';
    if(c < firstChar || c > lastChar) return nullptr;
  }
  stbtt_aligned_quad q;
  stbtt_GetBakedQuad(inner->CharData(), atlasWidth, atlasHeight, c - firstChar, &x, &y, &q, 1);
  if(xmax > 0.0f && x > xmax) {  // new line
    x = xmin, y += fontHeight;
    stbtt_GetBakedQuad(inner->CharData(), atlasWidth, atlasHeight, c - firstChar, &x, &y, &q, 1);
  }
  auto rectangle0 = make_shared<GLRectangle>(q.x1 - q.x0, q.y1 - q.y0);
  auto rectangle1 = make_shared<GLFontRangeTextureDecorator>(rectangle0, program, texture);
  rectangle1->SetTexCoords(q.s0, q.t0, q.s1, q.t1);
  mat4 transform = translate(mat4(1.0f), vec3((q.x0 + q.x1) * 0.5f, (q.y0 + q.y1) * 0.5f, 0.0f));
  return make_shared<GLTransformedRenderable>(rectangle1, transform);
#else  /* HAS_STB */
  return nullptr;
#endif /* HAS_STB */
}

GLRenderablePtr GLFontRange::GetRenderable(char c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const
{
  return GetRenderable((wchar_t)c, x, y, xmin, xmax);
}

GLRenderablePtr GLFontRange::GetRenderable(const wstring &s, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const
{
  GLCompositeRenderablePtr renderable = make_shared<GLCompositeRenderable>();
  for(wchar_t c : s) {
    auto r = GetRenderable(c, x, y, xmin, xmax);
    if(!r) continue;
    renderable->AddRenderable(r);
  }
  return renderable;
}

GLRenderablePtr GLFontRange::GetRenderable(const string &s, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const
{
  GLCompositeRenderablePtr renderable = make_shared<GLCompositeRenderable>();
  for(char c : s) {
    auto r = GetRenderable(c, x, y, xmin, xmax);
    if(!r) continue;
    renderable->AddRenderable(r);
  }
  return renderable;
}

void GLFontRange::Init(GLint width, GLint height)
{
  program = GLProgram::GetFontTextureProgram();
  Reshape(width, height);
  SetColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
  SetDepth(0.0f);
}

void GLFontRange::Reshape(GLint width, GLint height)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_winWidth", (GLfloat)width);
  program->SetUniform("u_winHeight", (GLfloat)height);
}

void GLFontRange::SetColor(const vec4 &color)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_color", color);
}

void GLFontRange::SetDepth(GLfloat depth)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_depth", depth);
}
