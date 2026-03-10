#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class KochSnowflake : public GLBasicGeometry {
public:
  KochSnowflake(GLint order);
  ~KochSnowflake() override;

protected:
  GLint order;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
