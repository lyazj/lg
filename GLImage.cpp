#include "GLImage.h"

#include <Magick++.h>

using namespace std;

void GLImage::Save(const fs::path &path) const
{
  const string map = (type == GLImageType::RGBA) ? "RGBA" : "RGB";
  Magick::Image img(
    Magick::Geometry(static_cast<size_t>(width), static_cast<size_t>(height)),
    Magick::Color("black"));
  img.depth(8);
  img.read(static_cast<size_t>(width), static_cast<size_t>(height),
           map, Magick::CharPixel, data.data());
  img.flip();
  img.write(path.string());
}

void GLImage::Load(const fs::path &path)
{
  Magick::Image img(path.string());
  img.flip();
  width  = static_cast<GLint>(img.columns());
  height = static_cast<GLint>(img.rows());
  // alpha() is the IM7 API; matte() is the IM6 API (returns true when alpha exists).
#if defined(MAGICKCORE_VERSION_MAJOR) && MAGICKCORE_VERSION_MAJOR >= 7
  const bool hasAlpha = img.alpha();
#else
  const bool hasAlpha = img.matte();
#endif
  type = hasAlpha ? GLImageType::RGBA : GLImageType::RGB;
  const string map      = hasAlpha ? "RGBA" : "RGB";
  const size_t channels = hasAlpha ? 4u : 3u;
  data.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * channels);
  img.write(0, 0,
    static_cast<size_t>(width), static_cast<size_t>(height),
    map, Magick::CharPixel, data.data());
}
