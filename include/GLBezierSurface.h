#pragma once

#include "GLBufferedGeometry.h"

class GLBezierSurface : public GL3DBufferedGeometry {
public:
  GLBezierSurface(vec3 controls[16], GLint division);
  ~GLBezierSurface() override;

protected:
  vec3 controls[16];
  GLint division;
  GLBuffer elementBuffer;
  std::vector<GLuint> elements;

  void IssueBuffer() const override;
  void IssueDraw() const override;
};
