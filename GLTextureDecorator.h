#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

GL_DECLARE_CLASS(GLSphere)

class GLTextureDecorator : public GLSimpleRenderableDecorator {
public:
  GLTextureDecorator(GLSimpleRenderablePtr r, GLProgramPtr p, GLTexturePtr t)
      : GLSimpleRenderableDecorator(std::move(r), std::move(p)), texture(std::move(t))
  {
    // empty
  }
  ~GLTextureDecorator() override = default;

  const GLTexturePtr &GetTexture() const { return texture; }
  void SetTexture(const GLTexturePtr &t) { texture = t; }

protected:
  GLTexturePtr texture;

  void IssueDraw(const mat4 &model) const override;
};

class GLBufferedTextureDecorator : public GLTextureDecorator {
public:
  using GLTextureDecorator::GLTextureDecorator;

  void Buffer() const override;

protected:
  GLBuffer texCoordBuffer;
  mutable std::vector<vec2> texCoords;

  void IssueDraw(const mat4 &model) const override;
  virtual void SetTexCoords() const = 0;
};

class GLPlanarTextureDecorator : public GLBufferedTextureDecorator {
public:
  using GLBufferedTextureDecorator::GLBufferedTextureDecorator;

protected:
  void SetTexCoords() const override;
};

class GLSphericalTextureDecorator : public GLBufferedTextureDecorator {
public:
  using GLBufferedTextureDecorator::GLBufferedTextureDecorator;

protected:
  void SetTexCoords() const override;
};
