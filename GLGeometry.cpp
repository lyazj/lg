#include "GLGeometry.h"

#include <glm/mat4x4.hpp>

using namespace std;

GLGeometry::GLGeometry()
{
  // empty
}

GLGeometry::~GLGeometry()
{
  // empty
}

void GLGeometry::Draw() const { Draw(glm::mat4(1.0)); }
