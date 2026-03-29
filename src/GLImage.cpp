#include "GLImage.h"

#include "GLApplication.h"
#include "Utils.h"

#ifdef HAS_MAGICK
#include <Magick++.h>
#endif /* HAS_MAGICK */

#ifdef HAS_STB
#ifdef NEED_STB_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif /* NEED_STB_IMPLEMENTATION */
#include <stb_image.h>
#include <stb_image_write.h>
#endif /* HAS_STB */

#include <fstream>
#include <iostream>

using namespace std;

class GLImage::Backend {
public:
  virtual ~Backend() = default;

  static void Init(GLImageBackend backend);
  static Backend &Get() { return *gInstance; }
  virtual void Load(GLImage &image, const fs::path &) const;
  virtual void Save(const GLImage &image, const fs::path &) const;

private:
  class GLImageMagickBackend;
  class GLImageSTBBackend;

  static unique_ptr<Backend> gInstance;
};

void GLImage::Init(GLImageBackend backend) { Backend::Init(backend); }

void GLImage::Load(const fs::path &path) { Backend::Get().Load(*this, path); }

void GLImage::Save(const fs::path &path) const { Backend::Get().Save(*this, path); }

#ifdef HAS_MAGICK

class GLImage::Backend::GLImageMagickBackend : public GLImage::Backend {
public:
  GLImageMagickBackend();
  void Load(GLImage &image, const fs::path &) const override;
  void Save(const GLImage &image, const fs::path &) const override;
};

GLImage::Backend::GLImageMagickBackend::GLImageMagickBackend()
{
  Magick::InitializeMagick(ToLocaleString(GLApplication::GetInstance()->GetProgramName()).c_str());
}

void GLImage::Backend::GLImageMagickBackend::Load(GLImage &image, const fs::path &path) const
{
  Magick::Image img(ToLocaleString(path));
  img.flip();
  image.width = (GLint)img.columns(), image.height = (GLint)img.rows();
  image.type = img.matte() ? GLImageType::RGBA : GLImageType::RGB;
  image.data.resize(image.width * image.height * (image.type == GLImageType::RGBA ? 4 : 3));
  const char *format = image.type == GLImageType::RGBA ? "RGBA" : "RGB";
  img.write(0, 0, image.width, image.height, format, Magick::CharPixel, image.data.data());
}

void GLImage::Backend::GLImageMagickBackend::Save(const GLImage &image, const fs::path &path) const
{
  const char *format = nullptr;
  switch(image.type) {
  case GLImageType::RGB: format = "RGB"; break;
  case GLImageType::RGBA: format = "RGBA"; break;
  default: abort();
  }
  Magick::Image img(image.width, image.height, format, Magick::CharPixel, image.data.data());
  img.flip();
  img.write(ToLocaleString(path));
}

#endif /* HAS_MAGICK */

#ifdef HAS_STB

class GLImage::Backend::GLImageSTBBackend : public GLImage::Backend {
public:
  GLImageSTBBackend();
  void Load(GLImage &image, const fs::path &) const override;
  void Save(const GLImage &image, const fs::path &) const override;
};

GLImage::Backend::GLImageSTBBackend::GLImageSTBBackend()
{
  stbi_set_flip_vertically_on_load(1);
  stbi_flip_vertically_on_write(1);
}

void GLImage::Backend::GLImageSTBBackend::Load(GLImage &image, const fs::path &path) const
{
  int n;
  unsigned char *d = stbi_load(ToLocaleString(path).c_str(), &image.width, &image.height, &n, 4);
  if(!d) {
    cerr << "Error loading image: " << ToLocaleString(path) << ": " << stbi_failure_reason() << endl;
    image.width = image.height = 0;
    image.data.clear();
    image.type = GLImageType::UNKNOWN;
    return;
  }
  image.type = GLImageType::RGBA;
  image.data.assign((const byte *)d, (const byte *)d + image.width * image.height * 4);
  stbi_image_free(d);
}

void GLImage::Backend::GLImageSTBBackend::Save(const GLImage &image, const fs::path &path) const
{
  int n;
  switch(image.type) {
  case GLImageType::RGB: n = 3; break;
  case GLImageType::RGBA: n = 4; break;
  default: abort();
  }
  if(stbi_write_png(ToLocaleString(path).c_str(), image.width, image.height, n, image.data.data(), 0) == 0) {
    cerr << "Error saving image: " << ToLocaleString(path) << endl;
  }
}

#endif /* HAS_STB */

unique_ptr<GLImage::Backend> GLImage::Backend::gInstance;

void GLImage::Backend::Init(GLImageBackend backend)
{
  if(gInstance) abort();

#ifndef HAS_MAGICK
  if(backend == GLImageBackend::Magick) {
    cerr << "Warning: Magick backend unavailable, falling back to default" << endl;
    backend = GLImageBackend::Default;
  }
#endif /* HAS_MAGICK */

#ifndef HAS_STB
  if(backend == GLImageBackend::STB) {
    cerr << "Warning: STB backend unavailable, falling back to default" << endl;
    backend = GLImageBackend::Default;
  }
#endif /* HAS_STB */

  if(backend == GLImageBackend::Default) {
#ifdef HAS_MAGICK
    backend = GLImageBackend::Magick;
#else /* HAS_MAGICK */
#ifdef HAS_STB
    backend = GLImageBackend::STB;
#endif /* HAS_STB */
#endif /* HAS_MAGICK */
  }

  switch(backend) {
  case GLImageBackend::Default:
    gInstance = make_unique<Backend>();
    cerr << "Warning: no GLImage backend available, using minimal builtin implementation" << endl;
    break;

#ifdef HAS_MAGICK
  case GLImageBackend::Magick: gInstance = make_unique<GLImageMagickBackend>(); break;
#endif /* HAS_MAGICK */

#ifdef HAS_STB
  case GLImageBackend::STB: gInstance = make_unique<GLImageSTBBackend>(); break;
#endif /* HAS_STB */

  default: abort();  // never reached
  }
}

namespace {

enum Endian { LittleEndian, BigEndian };

inline Endian ProbeEndian()
{
  uint16_t x = 1;
  return *(const uint8_t *)&x == 1 ? LittleEndian : BigEndian;
}

uint16_t ToLittleEndian16(uint16_t x)
{
  if(ProbeEndian() == LittleEndian) return x;
  return uint16_t((x >> 8) | (x << 8));
}

uint32_t ToLittleEndian32(uint32_t x)
{
  if(ProbeEndian() == LittleEndian) return x;
  return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

#pragma pack(push, 1)
struct BMPFileHeader {
  uint16_t bfType = 0x4D42;  // "BM"
  uint32_t bfSize = 0;       // Change me.
  uint16_t bfReserved1 = 0, bfReserved2 = 0;
  uint32_t bfOffBits = 54;

  void ToLittleEndian();
};
struct BMPInfoHeader {
  uint32_t biSize = 40;
  int32_t biWidth = 0, biHeight = 0;  // Change us.
  uint16_t biPlanes = 1;
  uint16_t biBitCount = 32;  // RGBA.
  uint32_t biCompression = 0;
  uint32_t biSizeImage = 0;                                // Change me.
  int32_t biXPelsPerMeter = 2835, biYPelsPerMeter = 2835;  // 72 DPI
  uint32_t biClrUsed = 0, biClrImportant = 0;

  void ToLittleEndian();
};
#pragma pack(pop)

static_assert(sizeof(BMPFileHeader) == 14);
static_assert(sizeof(BMPInfoHeader) == 40);

void BMPFileHeader::ToLittleEndian()
{
  bfType = ToLittleEndian16(bfType);
  bfSize = ToLittleEndian32(bfSize);
  bfReserved1 = ToLittleEndian16(bfReserved1);
  bfReserved2 = ToLittleEndian16(bfReserved2);
  bfOffBits = ToLittleEndian32(bfOffBits);
}

void BMPInfoHeader::ToLittleEndian()
{
  biSize = ToLittleEndian32(biSize);
  biWidth = ToLittleEndian32(biWidth);
  biHeight = ToLittleEndian32(biHeight);
  biPlanes = ToLittleEndian16(biPlanes);
  biBitCount = ToLittleEndian16(biBitCount);
  biCompression = ToLittleEndian32(biCompression);
  biSizeImage = ToLittleEndian32(biSizeImage);
  biXPelsPerMeter = ToLittleEndian32(biXPelsPerMeter);
  biYPelsPerMeter = ToLittleEndian32(biYPelsPerMeter);
  biClrUsed = ToLittleEndian32(biClrUsed);
  biClrImportant = ToLittleEndian32(biClrImportant);
}

}  // namespace

void GLImage::Backend::Load(GLImage &, const fs::path &) const
{
  cerr << "Warning: no GLImage backend available, Load() is a no-op" << endl;
}

void GLImage::Backend::Save(const GLImage &image, const fs::path &path) const
{
  BMPFileHeader fileHeader;
  BMPInfoHeader infoHeader;
  fileHeader.bfSize = fileHeader.bfOffBits + image.width * image.height * 4;
  infoHeader.biWidth = image.width;
  infoHeader.biHeight = image.height;
  infoHeader.biSizeImage = image.width * image.height * 4;
  fileHeader.ToLittleEndian();
  infoHeader.ToLittleEndian();

  ofstream file(path, ios::binary);
  file.write((const char *)&fileHeader, sizeof(fileHeader));
  file.write((const char *)&infoHeader, sizeof(infoHeader));
  int n = image.type == GLImageType::RGBA ? 4 : 3;
  for(GLint y = 0; y < image.height; y++) {
    for(GLint x = 0; x < image.width; x++) {
      const byte *p = &image.data[n * (y * image.width + x)];
      file.write((const char *)(p + 2), 1);                    // B
      file.write((const char *)(p + 1), 1);                    // G
      file.write((const char *)(p + 0), 1);                    // R
      file.write(n == 4 ? (const char *)(p + 3) : "\xff", 1);  // A
    }
  }
  if(!file) cerr << "Error saving image: " << ToLocaleString(path) << endl;
}
