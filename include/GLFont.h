#pragma once

#include <map>
#include <string>

#include "Global.h"

GL_DECLARE_CLASS(FileMap)
GL_DECLARE_CLASS(GLFontRange)

class GLFont {
public:
  GLFont(const fs::path &path, GLfloat fontH);
  ~GLFont();
  GLFont(const GLFont &) = delete;
  GLFont &operator=(const GLFont &) = delete;

  GLfloat GetFontHeight() const { return fontHeight; }
  GLFontRange *AddRange(GLint atlasW, GLint atlasH, wchar_t firstC, wchar_t lastC);

  GLRenderablePtr GetRenderable(wchar_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax);
  GLRenderablePtr GetRenderable(const std::wstring &s, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax);

  static const GLProgramPtr &GetProgram() { return program; }
  static void Init(GLint width, GLint height);
  static void Reshape(GLint width, GLint height);
  static void SetColor(const vec4 &color);
  static void SetDepth(GLfloat depth);  // NDC: -1 (near) to 1 (far)

private:
  FileMapUPtr fileMap;
  GLfloat fontHeight;
  std::map<wchar_t, GLFontRangeUPtr> fontRanges;

  static GLProgramPtr program;

  GLFontRange *HandleMissing(wchar_t c);
};
