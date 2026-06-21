#pragma once

#include "GLBufferedGeometry.h"

class GLBezierSurface : public GL3DBufferedGeometry {
public:
  GLBezierSurface(vec3 controls[16], GLint division);
  ~GLBezierSurface() override;

protected:
  vec3 controls[16];
  GLint division;
  GLBuffer normalBuffer;
  GLBuffer texCoordBuffer;
  GLBuffer elementBuffer;
  std::vector<vec3> normals;
  std::vector<vec2> texCoords;
  std::vector<GLuint> elements;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer() const override;
  void IssueDraw() const override;
};
