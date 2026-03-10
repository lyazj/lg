#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class GLCircle : public GLBasicGeometry {
public:
  GLCircle(GLfloat radius, GLint segments);
  ~GLCircle() override;

protected:
  GLfloat radius;
  GLint segments;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
