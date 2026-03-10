#pragma once

#include <glm/vec3.hpp>

#include "GLBasicGeometry.h"

class GLTriangle : public GLBasicGeometry {
public:
  GLTriangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3);
  ~GLTriangle() override;

protected:
  glm::vec3 vertices[3];

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
