#pragma once

#include "GLBufferedGeometry.h"

class GLSierpinskiGasketChaos : public GLBufferedGeometryD<2> {
public:
  GLSierpinskiGasketChaos(GLint points);
  ~GLSierpinskiGasketChaos() override;

protected:
  GLint points;

  void IssueDraw() const override;
};
