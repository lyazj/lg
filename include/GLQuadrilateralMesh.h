#pragma once

#include "GLBuffer.h"
#include "GLBufferedGeometry.h"

class GLQuadrilateralMesh : public GL3DBufferedGeometry {
public:
  GLQuadrilateralMesh(GLint stacks, GLint slices);
  ~GLQuadrilateralMesh() override;

  void Resize(GLint stacks, GLint slices);
  const vec3 *operator[](GLint row) const { return &vertices[row * (slices + 1)]; }
  vec3 *operator[](GLint row) { return (vec3 *)(*(const GLQuadrilateralMesh *)this)[row]; }

protected:
  GLint stacks, slices;

  mutable GLBuffer normalBuffer;
  GLBuffer texCoordBuffer;
  GLBuffer elementBuffer;
  mutable std::vector<vec3> normals;
  std::vector<vec2> texCoords;
  std::vector<GLuint> elements;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer() const override;
  void IssueDraw() const override;
};
