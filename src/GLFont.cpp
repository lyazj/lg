#include "GLFont.h"

#include <iomanip>
#include <iostream>

#include "FileMap.h"
#include "GLApplication.h"
#include "GLCompositeRenderable.h"
#include "GLFontRange.h"
#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLProgramPtr GLFont::program;

GLFont::GLFont(const fs::path &path, GLfloat fontH) : fileMap(make_unique<FileMap>(path)), fontHeight(fontH) { }

GLFont::~GLFont() { }

GLFontRange *GLFont::AddRange(GLint atlasW, GLint atlasH, char32_t firstC, char32_t lastC)
{
  auto fontRange = make_unique<GLFontRange>(*fileMap, fontHeight, atlasW, atlasH, firstC, lastC);
  return fontRanges.emplace(firstC, std::move(fontRange)).first->second.get();
}

GLRenderablePtr GLFont::GetRenderable(char32_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax)
{
  if(c == U'\n') {  // new line
    x = xmin, y += fontHeight;
    return nullptr;
  }
  auto it = fontRanges.upper_bound(c);
  GLFontRange *fontRange;
  if(it == fontRanges.begin() || prev(it)->second->GetLastChar() < c) {
    fontRange = HandleMissing(c);
  } else {
    fontRange = prev(it)->second.get();
  }
  return fontRange->GetRenderable(c, x, y, xmin, xmax);
}

GLRenderablePtr GLFont::GetRenderable(const u32string &s, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax)
{
  GLCompositeRenderablePtr renderable = make_shared<GLCompositeRenderable>();
  for(char32_t c : s) {
    GLRenderablePtr r = GetRenderable(c, x, y, xmin, xmax);
    if(!r) continue;
    renderable->AddRenderable(r);
  }
  return renderable;
}

void GLFont::Init(GLint width, GLint height)
{
  program = GLProgram::GetFontTextureProgram();
  Reshape(width, height);
  SetColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
  SetDepth(-0.999f);
}

void GLFont::Reshape(GLint width, GLint height)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_winWidth", (GLfloat)width);
  program->SetUniform("u_winHeight", (GLfloat)height);
}

void GLFont::SetColor(const vec4 &color)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_color", color);
}

void GLFont::SetDepth(GLfloat depth)
{
  GLProgramGuard guard(program);
  program->SetUniform("u_depth", depth);
}

GLFontRange *GLFont::HandleMissing(char32_t c)
{
  auto flags = clog.flags();
  char fill = clog.fill();
  string s = Narrow(ToWString(u32string(1, c)));
  clog << "Info: building glyph for '" << s << "' (U+" << hex << setw(4) << setfill('0') << (uint32_t)c << ")" << endl;
  clog.fill(fill);
  clog.flags(flags);

  // Bake a block of 256 glyphs including 'c'.
  // Assume glyphs are roughly square (width ≈ height) for layout purposes.
  // Add a safe margin by overestimating atlas size: allocate ~2× the estimated area.
  GLint index = GLint(ceilf(2.0f * logf(fontHeight) / logf(2.0f))) + 9;
  GLint hIndex = index / 2, wIndex = index - hIndex;
  return AddRange(1 << wIndex, 1 << hIndex, char32_t(c & -256), char32_t(c | 255));
}
