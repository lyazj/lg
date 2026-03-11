#pragma once

#include <glm/vec4.hpp>

#include "GLElementGeometry.h"

class GLAxes : public GLElementGeometryD<3> {
public:
  GLAxes(GLfloat x = 0.4f, GLfloat y = 0.4f, GLfloat z = 0.4f, const vec4 &xcolor = { 1.0f, 0.0f, 0.0f, 1.0f },
      const vec4 &ycolor = { 0.0f, 1.0f, 0.0f, 1.0f }, const vec4 &zcolor = { 0.0f, 0.0f, 1.0f, 1.0f });
  ~GLAxes() override;

  void SetVertexAttributes() const override;

protected:
  GLBuffer colorBuffer;
  std::vector<vec4> colors;

  void IssueBuffer() const override;
  void IssueDraw() const override;
};
