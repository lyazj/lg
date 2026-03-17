#pragma once

#include "GLBufferedGeometry.h"

class GLSierpinskiGasketChaos : public GL2DBufferedGeometry {
public:
  GLSierpinskiGasketChaos(GLint points);
  ~GLSierpinskiGasketChaos() override;

protected:
  GLint points;

  void IssueDraw() const override;
};
