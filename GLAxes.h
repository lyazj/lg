#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "GLBasicGeometry.h"

class GLAxes : public GLBasicGeometry {
public:
  GLAxes(GLfloat x = 0.4f, GLfloat y = 0.4f, GLfloat z = 0.4f, const glm::vec4 &xcolor = { 1.0f, 0.0f, 0.0f, 1.0f },
      const glm::vec4 &ycolor = { 0.0f, 1.0f, 0.0f, 1.0f }, const glm::vec4 &zcolor = { 0.0f, 0.0f, 1.0f, 1.0f });
  ~GLAxes() override;

  void SetVertexAttributes() const override;

protected:
  GLBuffer colorBuffer;

  glm::vec3 vertices[6];
  glm::vec4 colors[6];

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
