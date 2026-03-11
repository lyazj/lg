#pragma once

#include <glm/vec4.hpp>
#include <vector>

#include "GLBuffer.h"
#include "GLGeometry.h"

class GLColoredGeometry : public GLGeometry {
public:
  GLColoredGeometry(GLBasicGeometryPtr g) : geometry(std::move(g)) { }
  ~GLColoredGeometry() override = default;

  const GLBasicGeometryPtr &GetGeometry() const { return geometry; }
  void SetGeometry(const GLBasicGeometryPtr &g) { geometry = g; }

  virtual void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  GLBasicGeometryPtr geometry;
  GLBuffer colorBuffer;
  mutable std::vector<vec4> colors;

  virtual void Color() const = 0;
};

class GLSingleColorGeometry : public GLColoredGeometry {
public:
  GLSingleColorGeometry(GLBasicGeometryPtr g, const vec4 &c) : GLColoredGeometry(std::move(g)), color(c) { }
  ~GLSingleColorGeometry() override = default;

  const vec4 &GetColor() const { return color; }
  void SetColor(const vec4 &c) { color = c; }

protected:
  vec4 color;

  void Color() const override;
};
