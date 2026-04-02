#pragma once

#include "GLCompositeRenderable.h"

class GLCuboid : public GLCompositeRenderable {
public:
  GLCuboid(GLfloat length, GLfloat width, GLfloat height);
  ~GLCuboid() override;

  GLSimpleRenderablePtr GetFace(GLint i) const;
  void SetFace(GLint i, const GLSimpleRenderablePtr &r);
  void GetVertex(GLint i, GLint j, vec3 &v) const;

protected:
  GLfloat length, width, height;

private:
  using GLCompositeRenderable::AddRenderable;
  using GLCompositeRenderable::GetRenderable;
  using GLCompositeRenderable::Reserve;
  using GLCompositeRenderable::SetRenderable;
};
