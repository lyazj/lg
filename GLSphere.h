#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "GLBasicGeometry.h"
#include "GLBuffer.h"

class GLSphere : public GLBasicGeometry {
public:
  GLSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLSphere() override;

protected:
  GLfloat radius;
  GLint slices, stacks;
  GLBuffer elementBuffer;
  std::vector<glm::vec3> vertices;
  std::vector<GLuint> elements;

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
