#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

class GLColorDecorator : public GLSimpleRenderableDecorator {
public:
  GLColorDecorator(GLSimpleRenderablePtr r) : GLSimpleRenderableDecorator(std::move(r)) { }
  ~GLColorDecorator() override = default;

  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  virtual void Color() const = 0;
};

class GLFlatColorDecorator : public GLColorDecorator {
public:
  GLFlatColorDecorator(GLSimpleRenderablePtr r, const vec4 &c) : GLColorDecorator(std::move(r)), color(c) { }
  ~GLFlatColorDecorator() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

protected:
  vec4 color;

  void Color() const override;
};

class GLRandomColorDecorator : public GLColorDecorator {
public:
  GLRandomColorDecorator(GLSimpleRenderablePtr r) : GLColorDecorator(std::move(r)) { }
  ~GLRandomColorDecorator() override = default;

protected:
  void Color() const override;
};
