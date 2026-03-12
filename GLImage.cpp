#include "GLImage.h"

#include <Magick++.h>  // graphicsmagick-libmagick-dev-compat

using namespace std;

void GLImage::Load(const fs::path &path)
{
  // We do need .string() on Windows unfortunately...
  Magick::Image img(path.string());
  width = (GLint)img.columns(), height = (GLint)img.rows();
  type = img.matte() ? GLImageType::RGBA : GLImageType::RGB;
  data.resize(width * height * (type == GLImageType::RGBA ? 4 : 3));
  const char *format = type == GLImageType::RGBA ? "RGBA" : "RGB";
  img.write(0, 0, width, height, format, Magick::CharPixel, data.data());
}

void GLImage::Save(const fs::path &path) const
{
  const char *format = NULL;
  switch(type) {
  case GLImageType::RGB: format = "RGB"; break;
  case GLImageType::RGBA: format = "RGBA"; break;
  default: abort();
  }
  Magick::Image(width, height, format, Magick::CharPixel, data.data()).write(path);
}
