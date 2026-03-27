#pragma once

#include <string>
#include <vector>

#include "Global.h"

class GLFont {
public:
  GLFont(const fs::path &path, GLfloat fontH, GLint atlasW, GLint atlasH, GLint firstC, GLint nC);
  ~GLFont();
  GLFont(const GLFont &) = delete;
  GLFont &operator=(const GLFont &) = delete;

  GLfloat GetFontHeight() const { return fontHeight; }
  GLint GetAtlasWidth() const { return atlasWidth; }
  GLint GetAtlasHeight() const { return atlasHeight; }
  GLint GetFirstChar() const { return firstChar; }
  GLint GetNChar() const { return nChar; }
  const GLTexturePtr &GetTexture() const { return texture; }

  GLSimpleRenderablePtr GetRenderable(GLint c, GLfloat &x, GLfloat &y) const;
  GLSimpleRenderablePtr GetRenderable(char c, GLfloat &x, GLfloat &y) const;
  GLRenderablePtr GetRenderable(const std::vector<GLint> &s, GLfloat &x, GLfloat &y, GLfloat xmax = 0.0f) const;
  GLRenderablePtr GetRenderable(const std::string &s, GLfloat &x, GLfloat &y, GLfloat xmax = 0.0f) const;

  static const GLProgramPtr &GetProgram() { return program; }
  static void Init(GLint width, GLint height);
  static void Resize(GLint width, GLint height);
  static void SetColor(const vec4 &color);
  static void SetDepth(GLfloat depth);

private:
  GLfloat fontHeight;
  GLint atlasWidth, atlasHeight, firstChar, nChar;
  GLTexturePtr texture;
  static GLProgramPtr program;

  class Inner;
  Inner *inner;
};
