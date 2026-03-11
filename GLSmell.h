#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

class GLSmell : public GLBasicGeometry {
public:
  GLSmell(GLfloat radius, GLint segments);
  ~GLSmell() override;

protected:
  GLfloat radius;
  GLint segments;
  std::vector<glm::vec2> vertices;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
