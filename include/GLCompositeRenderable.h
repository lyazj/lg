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

protected:
  std::vector<GLRenderablePtr> renderables;

  void IssueSetVertexAttributes(bool force) const override;
  void IssueBuffer(bool force) const override;
  void IssueDraw(const mat4 &model) const override;
};

inline void GLCompositeRenderable::IssueDraw(const mat4 &model) const
{
  for(const GLRenderablePtr &r : renderables) r->Draw(model);
}

inline void GLCompositeRenderable::IssueSetVertexAttributes(bool force) const
{
  for(const GLRenderablePtr &r : renderables) r->SetVertexAttributes(force);
}

inline void GLCompositeRenderable::IssueBuffer(bool force) const
{
  for(const GLRenderablePtr &r : renderables) r->Buffer(force);
}
