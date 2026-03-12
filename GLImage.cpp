#include "GLImage.h"

#include <Magick++.h>

using namespace std;

void GLImage::Load(const fs::path &path)
{
  Magick::Image img(path);
  width = (GLint)img.columns(), height = (GLint)img.rows();
  type = img.hasChannel(Magick::AlphaPixelChannel) ? GLImageType::RGBA : GLImageType::RGB;
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
