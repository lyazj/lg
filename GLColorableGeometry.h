#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

class GLColorableGeometry : public GLSimpleRenderableDecorator {
public:
  GLColorableGeometry(GLSimpleRenderablePtr g) : GLSimpleRenderableDecorator(std::move(g)) { }
  ~GLColorableGeometry() override = default;

  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  virtual void Color() const = 0;
};

class GLFlatColorGeometry : public GLColorableGeometry {
public:
  GLFlatColorGeometry(GLSimpleRenderablePtr g, const vec4 &c) : GLColorableGeometry(std::move(g)), color(c) { }
  ~GLFlatColorGeometry() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

protected:
  vec4 color;

  void Color() const override;
};

class GLRandomColorGeometry : public GLColorableGeometry {
public:
  GLRandomColorGeometry(GLSimpleRenderablePtr g) : GLColorableGeometry(std::move(g)) { }
  ~GLRandomColorGeometry() override = default;

protected:
  void Color() const override;
};
