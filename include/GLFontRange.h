#pragma once

#include "Global.h"

class FileMap;

class GLFontRange {
public:
  GLFontRange(const FileMap &fMap, GLfloat fontH, GLint atlasW, GLint atlasH, char32_t firstC, char32_t lastC);
  ~GLFontRange();
  GLFontRange(const GLFontRange &) = delete;
  GLFontRange &operator=(const GLFontRange &) = delete;

  GLfloat GetFontHeight() const { return fontHeight; }
  GLint GetAtlasWidth() const { return atlasWidth; }
  GLint GetAtlasHeight() const { return atlasHeight; }
  char32_t GetFirstChar() const { return firstChar; }
  char32_t GetLastChar() const { return lastChar; }
  const GLTexturePtr &GetTexture() const { return texture; }

  GLRenderablePtr GetRenderable(char32_t c, GLfloat &x, GLfloat &y, GLfloat xmin, GLfloat xmax) const;

private:
  GLfloat fontHeight;
  GLint atlasWidth, atlasHeight;
  char32_t firstChar, lastChar;
  GLTexturePtr texture;

  class Inner;
  Inner *inner;
};
