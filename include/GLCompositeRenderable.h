#pragma once

#include <vector>

#include "GLRenderable.h"

class GLCompositeRenderable : public GLRenderable {
public:
  GLCompositeRenderable() = default;
  ~GLCompositeRenderable() override = default;

  void Reserve(GLint n) { renderables.reserve(n); }
  GLint GetNRenderable() const { return (GLint)renderables.size(); }
  const GLRenderablePtr &GetRenderable(GLint i) const { return renderables.at(i); }
  void SetRenderable(GLint i, const GLRenderablePtr &r) { renderables.at(i) = r; }
  void AddRenderable(const GLRenderablePtr &r) { renderables.push_back(r); }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  std::vector<GLRenderablePtr> renderables;
};

inline void GLCompositeRenderable::SetVertexAttributes() const
{
  for(const GLRenderablePtr &r : renderables) r->SetVertexAttributes();
}

inline void GLCompositeRenderable::Buffer() const
{
  for(const GLRenderablePtr &r : renderables) r->Buffer();
}

inline void GLCompositeRenderable::Draw(const mat4 &model) const
{
  for(const GLRenderablePtr &r : renderables) r->Draw(model);
}
