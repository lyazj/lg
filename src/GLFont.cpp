#include "GLFont.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "FileMap.h"
#include "GLCompositeRenderable.h"
#include "GLProgram.h"
#include "GLRectangle.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "GLTransformedRenderable.h"

#ifdef HAS_STB
#include <stb/stb_truetype.h>
#endif /* HAS_STB */

using namespace std;

GLProgramPtr GLFont::program;

namespace {

class GLFontTextureDecorator : public GLBufferedTextureDecorator {
public:
  GLFontTextureDecorator(GLSimpleRenderablePtr r, GLProgramPtr p, GLTexturePtr t)
      : GLBufferedTextureDecorator(std::move(r), std::move(p), std::move(t))
  {
    // empty
  }
  ~GLFontTextureDecorator() override = default;

  void SetTexCoords(GLfloat s0, GLfloat t0, GLfloat s1, GLfloat t1)
  {
    texCoords = { { s0, t0 }, { s0, t1 }, { s1, t0 }, { s1, t1 } };
  }

protected:
  virtual void SetTexCoords() const override { }
};

}  // namespace

#ifdef HAS_STB
class GLFont::Inner {
public:
  Inner(GLint nChar) : charData(new stbtt_bakedchar[nChar]) { }
  ~Inner() { delete[] charData; }
  Inner(const Inner &) = delete;
  Inner &operator=(const Inner &) = delete;

  stbtt_bakedchar *CharData() { return charData; }
  const stbtt_bakedchar *CharData() const { return charData; }

private:
  stbtt_bakedchar *charData;
};
#else  /* HAS_STB */
class GLFont::Inner { };
#endif /* HAS_STB */

GLFont::GLFont(const fs::path &path, GLfloat fontH, GLint atlasW, GLint atlasH, GLint firstC, GLint nC)
    : fontHeight(fontH), atlasWidth(atlasW), atlasHeight(atlasH), firstChar(firstC), nChar(nC)
{
  inner = new Inner(nChar);
  vector<byte> data(atlasW * atlasH);

#ifdef HAS_STB
  FileMap fMap(path);
  stbtt_BakeFontBitmap((unsigned char *)fMap.data(), 0, fontHeight, (unsigned char *)data.data(), atlasWidth,
      atlasHeight, firstChar, nChar, inner->CharData());
#else  /* HAS_STB */
  cerr << "Warning: GLFont: STB not available, font loading not supported" << endl;
#endif /* HAS_STB */

  texture = make_shared<GLTexture>();
  texture->Texture(GL_RED, atlasWidth, atlasHeight, GL_RED, data.data());
}

GLFont::~GLFont() { delete inner; }

GLSimpleRenderablePtr GLFont::GetRenderable(GLint c, GLfloat &x, GLfloat &y) const
{
#ifdef HAS_STB
  if(c < firstChar || c >= firstChar + nChar) {
    cerr << "Warning: GLFont: character " << c << " out of range" << endl;
    c = '?';
  }
  stbtt_aligned_quad q;
  stbtt_GetBakedQuad(inner->CharData(), atlasWidth, atlasHeight, c - firstChar, &x, &y, &q, 1);

  auto rectangle0 = make_shared<GLRectangle>(q.x1 - q.x0, q.y1 - q.y0);
  auto rectangle1 = make_shared<GLFontTextureDecorator>(rectangle0, program, texture);
  rectangle1->SetTexCoords(q.s0, q.t0, q.s1, q.t1);
  return rectangle1;
#else  /* HAS_STB */
  return nullptr;
#endif /* HAS_STB */
}

GLSimpleRenderablePtr GLFont::GetRenderable(char c, GLfloat &x, GLfloat &y) const
{
  return GetRenderable((GLint)c, x, y);
}

GLRenderablePtr GLFont::GetRenderable(const vector<GLint> &s, GLfloat &x, GLfloat &y, GLfloat xmax) const
{
  GLCompositeRenderablePtr renderable = make_shared<GLCompositeRenderable>();
  for(GLint c : s) {
    if(c == '\n') {  // new line
      x = 0.0f, y += fontHeight;
      continue;
    }
    GLfloat x0 = x, y0 = y;
    auto rectangle0 = GetRenderable(c, x, y);
    if(!rectangle0) continue;
    if(xmax > 0.0f && x > xmax) {  // new line
      x -= x0, y += fontHeight;
      x0 = 0.0f, y0 = y;
    }
    mat4 transform = translate(mat4(1.0f), vec3(x0, y0, 0.0f));
    auto rectangle1 = make_shared<GLTransformedRenderable>(rectangle0, transform);
    renderable->AddRenderable(rectangle1);
  }
  return renderable;
}

GLRenderablePtr GLFont::GetRenderable(const string &s, GLfloat &x, GLfloat &y, GLfloat xmax) const
{
  return GetRenderable(vector<GLint>(s.begin(), s.end()), x, y, xmax);
}

void GLFont::Init(GLint width, GLint height)
{
  program = GLProgram::GetFontTextureProgram();
  Resize(width, height);
  SetColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

void GLFont::Resize(GLint width, GLint height)
{
  program->SetUniform("u_winWidth", width);
  program->SetUniform("u_winHeight", height);
}

void GLFont::SetColor(const vec4 &color) { program->SetUniform("u_color", color); }
