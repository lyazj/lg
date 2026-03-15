#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

class GLTextureDecorator : public GLSimpleRenderableDecorator {
public:
  GLTextureDecorator(GLSimpleRenderablePtr r, GLTexturePtr t, GLProgramPtr p)
      : GLSimpleRenderableDecorator(std::move(r)), texture(std::move(t)), program(std::move(p))
  {
    // empty
  }
  ~GLTextureDecorator() override = default;

  const GLTexturePtr &GetTexture() const { return texture; }
  void SetTexture(const GLTexturePtr &t) { texture = t; }

  void Draw(const mat4 &model) const override;

protected:
  GLTexturePtr texture;
  GLProgramPtr program;
};

class GLBufferedTextureDecorator : public GLTextureDecorator {
public:
  GLBufferedTextureDecorator(GLSimpleRenderablePtr r, GLTexturePtr t, GLProgramPtr p)
      : GLTextureDecorator(std::move(r), std::move(t), std::move(p))
  {
    // empty
  }
  ~GLBufferedTextureDecorator() override = default;

  void Buffer() const override;

protected:
  GLBuffer texCoordBuffer;
  mutable std::vector<vec2> texCoords;

  virtual void SetTexCoords() const = 0;
};

class GL2DTextureDecorator : public GLBufferedTextureDecorator {
public:
  GL2DTextureDecorator(GLSimpleRenderablePtr r, GLTexturePtr t, GLProgramPtr p)
      : GLBufferedTextureDecorator(std::move(r), std::move(t), std::move(p))
  {
    // empty
  }
  ~GL2DTextureDecorator() override = default;

protected:
  void SetTexCoords() const override;
};
