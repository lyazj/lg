#include "GLImage.h"

#include "GLApplication.h"

#ifdef HAS_MAGICK
#include <Magick++.h>
#endif /* HAS_MAGICK */

#ifdef HAS_STB
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#endif /* HAS_STB */

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
  Magick::InitializeMagick(GLApplication::GetInstance()->GetProgramName().c_str());
}

void GLImage::Backend::GLImageMagickBackend::Load(
    GLImage &image [[maybe_unused]], const fs::path &path [[maybe_unused]]) const
{
  // We do need .string() on Windows unfortunately...
  Magick::Image img(path.string());
  img.flip();
  image.width = (GLint)img.columns(), image.height = (GLint)img.rows();
  //image.type = img.matte() ? GLImageType::RGBA : GLImageType::RGB;
  image.type = GLImageType::RGBA;  // Force RGBA for simplicity.
  image.data.resize(image.width * image.height * (image.type == GLImageType::RGBA ? 4 : 3));
  const char *format = image.type == GLImageType::RGBA ? "RGBA" : "RGB";
  img.write(0, 0, image.width, image.height, format, Magick::CharPixel, image.data.data());
}

void GLImage::Backend::GLImageMagickBackend::Save(
    const GLImage &image [[maybe_unused]], const fs::path &path [[maybe_unused]]) const
{
  const char *format = nullptr;
  switch(image.type) {
  case GLImageType::RGB: format = "RGB"; break;
  case GLImageType::RGBA: format = "RGBA"; break;
  default: abort();
  }
  Magick::Image img(image.width, image.height, format, Magick::CharPixel, image.data.data());
  img.flip();
  img.write(path.string());
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

void GLImage::Backend::GLImageSTBBackend::Load(
    GLImage &image [[maybe_unused]], const fs::path &path [[maybe_unused]]) const
{
  int n;
  unsigned char *d = stbi_load(path.string().c_str(), &image.width, &image.height, &n, 4);
  if(!d) {
    cerr << "Error loading image: " << path << ": " << stbi_failure_reason() << endl;
    image.width = image.height = 0;
    image.data.clear();
    image.type = GLImageType::UNKNOWN;
    return;
  }
  image.type = GLImageType::RGBA;
  image.data.assign((const byte *)d, (const byte *)d + image.width * image.height * n);
  stbi_image_free(d);
}

void GLImage::Backend::GLImageSTBBackend::Save(
    const GLImage &image [[maybe_unused]], const fs::path &path [[maybe_unused]]) const
{
  int n;
  switch(image.type) {
  case GLImageType::RGB: n = 3; break;
  case GLImageType::RGBA: n = 4; break;
  default: abort();
  }
  if(stbi_write_png(path.string().c_str(), image.width, image.height, n, image.data.data(), 0) == 0) {
    cerr << "Error saving image: " << path << endl;
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
    cerr << "Warning: no GLImage backend available, using dummy implementation" << endl;
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

void GLImage::Backend::Load(GLImage &, const fs::path &) const
{
  cerr << "Warning: no GLImage backend available, Load() is a no-op" << endl;
}

void GLImage::Backend::Save(const GLImage &, const fs::path &) const
{
  cerr << "Warning: no GLImage backend available, Save() is a no-op" << endl;
}
