#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

class GLColorDecorator : public GLSimpleRenderableDecorator {
public:
  GLColorDecorator(GLSimpleRenderablePtr r) : GLSimpleRenderableDecorator(std::move(r)) { }
  ~GLColorDecorator() override = default;
};

class GLUniformColorDecorator : public GLColorDecorator {
public:
  GLUniformColorDecorator(GLSimpleRenderablePtr r, const vec4 &c) : GLColorDecorator(std::move(r)), color(c) { }
  ~GLUniformColorDecorator() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

  void Draw(const mat4 &model) const override;

protected:
  vec4 color;
};

class GLBufferedColorDecorator : public GLColorDecorator {
public:
  GLBufferedColorDecorator(GLSimpleRenderablePtr r) : GLColorDecorator(std::move(r)) { }
  ~GLBufferedColorDecorator() override = default;

  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  virtual void SetColor() const = 0;
};

class GLRandomColorDecorator : public GLBufferedColorDecorator {
public:
  GLRandomColorDecorator(GLSimpleRenderablePtr r) : GLBufferedColorDecorator(std::move(r)) { }
  ~GLRandomColorDecorator() override = default;

protected:
  void SetColor() const override;
};
