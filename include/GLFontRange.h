#pragma once

#include "Global.h"

class FileMap;

class GLFontRange {
public:
  GLFontRange(const FileMap &fMap, GLfloat fontH, GLint atlasW, GLint atlasH, wchar_t firstC, wchar_t lastC);
  ~GLFontRange();
  GLFontRange(const GLFontRange &) = delete;
  GLFontRange &operator=(const GLFontRange &) = delete;

  GLfloat GetFontHeight() const { return fontHeight; }
  GLint GetAtlasWidth() const { return atlasWidth; }
  GLint GetAtlasHeight() const { return atlasHeight; }
  wchar_t GetFirstChar() const { return firstChar; }
  wchar_t GetLastChar() const { return lastChar; }
  const GLTexturePtr &GetTexture() const { return texture; }

  GLRenderablePtr GetRenderable(wchar_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const;

private:
  GLfloat fontHeight;
  GLint atlasWidth, atlasHeight;
  wchar_t firstChar, lastChar;
  GLTexturePtr texture;

  class Inner;
  Inner *inner;
};
