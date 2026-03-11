#pragma once
#include <vector>

#include "GLRenderable.h"

class GLCompositeRenderable : public GLRenderable {
public:
  GLCompositeRenderable() = default;
  ~GLCompositeRenderable() override = default;

  GLuint GetNGeometry() const { return (GLuint)geometries.size(); }
  const GLRenderablePtr &GetGeometry(GLuint i) const { return geometries.at(i); }
  void AddGeometry(const GLRenderablePtr &g) { geometries.push_back(g); }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  std::vector<GLRenderablePtr> geometries;
};

inline void GLCompositeRenderable::SetVertexAttributes() const
{
  for(const GLRenderablePtr &g : geometries) g->SetVertexAttributes();
}

inline void GLCompositeRenderable::Buffer() const
{
  for(const GLRenderablePtr &g : geometries) g->Buffer();
}

inline void GLCompositeRenderable::Draw(const mat4 &model) const
{
  for(const GLRenderablePtr &g : geometries) g->Draw(model);
}
