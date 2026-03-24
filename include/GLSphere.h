#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLSphere : public GL3DBufferedGeometry {
public:
  GLSphere(GLfloat radius, GLint slices, GLint stacks);
  ~GLSphere() override;

  void SetVertexAttributes() const override;

protected:
  GLfloat radius;
  GLint slices, stacks;
  GLBuffer normalBuffer;
  GLBuffer texCoordBuffer;
  GLBuffer elementBuffer;
  std::vector<vec3> normals;
  std::vector<vec2> texCoords;
  std::vector<GLuint> elements;

  void IssueBuffer() const override;
  void IssueDraw() const override;
};
