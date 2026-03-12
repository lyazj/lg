#include "GLImage.h"

#ifdef HAS_MAGICK
#include <Magick++.h>
#endif /* HAS_MAGICK */

#include <iostream>

using namespace std;

void GLImage::Init(const char *name [[maybe_unused]])
{
#ifdef HAS_MAGICK
  Magick::InitializeMagick(name);
#endif /* HAS_MAGICK */
}

void GLImage::Load(const fs::path &path [[maybe_unused]])
{
#ifdef HAS_MAGICK
  // We do need .string() on Windows unfortunately...
  Magick::Image img(path.string());
  width = (GLint)img.columns(), height = (GLint)img.rows();
  type = img.matte() ? GLImageType::RGBA : GLImageType::RGB;
  data.resize(width * height * (type == GLImageType::RGBA ? 4 : 3));
  const char *format = type == GLImageType::RGBA ? "RGBA" : "RGB";
  img.write(0, 0, width, height, format, Magick::CharPixel, data.data());
#else  /* HAS_MAGICK */
  cerr << "Warning: GLImage::Load() unavailable" << endl;
#endif /* HAS_MAGICK */
}

void GLImage::Save(const fs::path &path [[maybe_unused]]) const
{
#ifdef HAS_MAGICK
  const char *format = NULL;
  switch(type) {
  case GLImageType::RGB: format = "RGB"; break;
  case GLImageType::RGBA: format = "RGBA"; break;
  default: abort();
  }
  Magick::Image(width, height, format, Magick::CharPixel, data.data()).write(path.string());
#else  /* HAS_MAGICK */
  cerr << "Warning: GLImage::Save() unavailable" << endl;
#endif /* HAS_MAGICK */
}
