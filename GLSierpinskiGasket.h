#pragma once

#include "GLBufferedGeometry.h"

class GLSierpinskiGasket : public GLBufferedGeometryD<2> {
public:
  GLSierpinskiGasket(GLint order, GLfloat noise = 0.0f);
  ~GLSierpinskiGasket() override;

protected:
  GLint order;
  GLfloat noise;

  void IssueDraw() const override;
};
