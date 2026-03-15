#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

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

  void Draw(const mat4 &model) const override;

protected:
  GLTexturePtr texture;
};

class GLBufferedTextureDecorator : public GLTextureDecorator {
public:
  using GLTextureDecorator::GLTextureDecorator;

  void Buffer() const override;

protected:
  GLBuffer texCoordBuffer;
  mutable std::vector<vec2> texCoords;

  virtual void SetTexCoords() const = 0;
};

class GL2DTextureDecorator : public GLBufferedTextureDecorator {
public:
  using GLBufferedTextureDecorator::GLBufferedTextureDecorator;

protected:
  void SetTexCoords() const override;
};
