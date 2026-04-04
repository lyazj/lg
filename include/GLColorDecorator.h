#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLSimpleRenderableDecorator.h"

class GLColorDecorator : public GLSimpleRenderableDecorator {
public:
  GLColorDecorator(GLSimpleRenderablePtr r) : GLSimpleRenderableDecorator(std::move(r), nullptr) { }
  ~GLColorDecorator() override = default;
};

class GLUniformColorDecorator : public GLColorDecorator {
public:
  GLUniformColorDecorator(GLSimpleRenderablePtr r, const vec4 &c) : GLColorDecorator(std::move(r)), color(c) { }
  ~GLUniformColorDecorator() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

protected:
  vec4 color;

  void IssueDraw(const mat4 &model) const override;
};

class GLBufferedColorDecorator : public GLColorDecorator {
public:
  using GLColorDecorator::GLColorDecorator;

protected:
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  void IssueBuffer(bool force) const override;
  void IssueDraw(const mat4 &model) const override;
  virtual void SetColors() const = 0;
};

class GLRandomColorDecorator : public GLBufferedColorDecorator {
public:
  using GLBufferedColorDecorator::GLBufferedColorDecorator;

protected:
  void SetColors() const override;
};
