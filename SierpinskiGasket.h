#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class SierpinskiGasket : public GLBasicGeometry {
public:
  SierpinskiGasket(GLint order, GLfloat noise = 0.0f);
  ~SierpinskiGasket() override;

protected:
  GLint order;
  GLfloat noise;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
