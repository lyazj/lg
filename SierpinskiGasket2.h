#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class SierpinskiGasket2 : public GLBasicGeometry {
public:
  SierpinskiGasket2(GLint points);
  ~SierpinskiGasket2() override;

protected:
  GLint points;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
