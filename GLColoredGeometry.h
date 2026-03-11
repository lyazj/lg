#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLPrimitiveGeometryDecorator.h"

class GLColoredGeometry : public GLPrimitiveGeometryDecorator {
public:
  GLColoredGeometry(GLPrimitiveGeometryPtr g) : GLPrimitiveGeometryDecorator(std::move(g)) { }
  ~GLColoredGeometry() override = default;

  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  virtual void Color() const = 0;
};

class GLSingleColorGeometry : public GLColoredGeometry {
public:
  GLSingleColorGeometry(GLPrimitiveGeometryPtr g, const vec4 &c) : GLColoredGeometry(std::move(g)), color(c) { }
  ~GLSingleColorGeometry() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

protected:
  vec4 color;

  void Color() const override;
};

class GLRandomColorGeometry : public GLColoredGeometry {
public:
  GLRandomColorGeometry(GLPrimitiveGeometryPtr g) : GLColoredGeometry(std::move(g)) { }
  ~GLRandomColorGeometry() override = default;

protected:
  void Color() const override;
};
