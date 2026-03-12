#pragma once

#include <vector>

#include "Global.h"

enum class GLImageType {
  UNKNOWN,
  RGB,
  RGBA,
};

class GLImage {
public:
  void Load(const fs::path &path);
  void Save(const fs::path &path) const;

  GLint GetWidth() const { return width; }
  void SetWidth(GLint w) { width = w; }
  GLint GetHeight() const { return height; }
  void SetHeight(GLint h) { height = h; }

  const std::vector<byte> &GetData() const { return data; }
  void SetData(std::vector<byte> b) { data = std::move(b); }
  GLImageType GetType() const { return type; }
  void SetType(GLImageType t) { type = t; }

private:
  GLint width = 0, height = 0;
  std::vector<byte> data;
  GLImageType type = GLImageType::UNKNOWN;
};
