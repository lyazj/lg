#include "GLFont.h"

#include "FileMap.h"
#include "GLApplication.h"
#include "GLCompositeRenderable.h"
#include "GLFontRange.h"
#include "GLProgram.h"

using namespace std;

GLProgramPtr GLFont::program;

GLFont::GLFont(const fs::path &path, GLfloat fontH) : fileMap(make_unique<FileMap>(path)), fontHeight(fontH) { }

GLFont::~GLFont() { }

GLFontRange *GLFont::AddRange(GLint atlasW, GLint atlasH, wchar_t firstC, wchar_t lastC)
{
  auto fontRange = make_unique<GLFontRange>(*fileMap, fontHeight, atlasW, atlasH, firstC, lastC);
  return fontRanges.emplace(firstC, std::move(fontRange)).first->second.get();
}

GLRenderablePtr GLFont::GetRenderable(wchar_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax)
{
  if(c == '\n') {  // new line
    x = xmin, y += fontHeight;
    return nullptr;
  }
  auto it = fontRanges.upper_bound(c);
  GLFontRange *fontRange;
  if(it == fontRanges.begin() || prev(it)->second->GetLastChar() < c) {
    fontRange = AddRange(GLint(ceilf(2.0f * fontHeight)), GLint(ceilf(2.0f * fontHeight)), c, c);
  } else {
    fontRange = prev(it)->second.get();
  }
  return fontRange->GetRenderable(c, x, y, xmin, xmax);
}

GLRenderablePtr GLFont::GetRenderable(const std::wstring &s, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax)
{
  GLCompositeRenderablePtr renderable = make_shared<GLCompositeRenderable>();
  for(wchar_t c : s) {
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
  SetDepth(0.0f);
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
