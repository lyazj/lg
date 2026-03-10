#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class BarnsleyFern : public GLBasicGeometry {
public:
  BarnsleyFern(GLint points);
  ~BarnsleyFern() override;

  void Normalize();

protected:
  GLint points;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
