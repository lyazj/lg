#pragma once

#include <vector>

#include "GLRenderable.h"

class GLCompositeRenderable : public GLRenderable {
public:
  GLCompositeRenderable() = default;
  ~GLCompositeRenderable() override = default;

  void Reserve(GLuint n) { geometries.reserve(n); }
  GLuint GetNRenderable() const { return (GLuint)geometries.size(); }
  const GLRenderablePtr &GetRenderable(GLuint i) const { return geometries.at(i); }
  void AddRenderable(const GLRenderablePtr &r) { geometries.push_back(r); }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  std::vector<GLRenderablePtr> geometries;
};

inline void GLCompositeRenderable::SetVertexAttributes() const
{
  for(const GLRenderablePtr &r : geometries) r->SetVertexAttributes();
}

inline void GLCompositeRenderable::Buffer() const
{
  for(const GLRenderablePtr &r : geometries) r->Buffer();
}

inline void GLCompositeRenderable::Draw(const mat4 &model) const
{
  for(const GLRenderablePtr &r : geometries) r->Draw(model);
}
