#pragma once
#include <vector>

#include "GLGeometry.h"

class GLCompositeGeometry : public GLGeometry {
public:
  GLCompositeGeometry() = default;
  ~GLCompositeGeometry() override = default;

  GLuint GetNGeometry() const { return (GLuint)geometries.size(); }
  const GLGeometryPtr &GetGeometry(GLuint i) const { return geometries.at(i); }
  void AddGeometry(const GLGeometryPtr &g) { geometries.push_back(g); }

  void SetVertexAttributes() const override;
  void Buffer() const override;
  void Draw(const mat4 &model) const override;

protected:
  std::vector<GLGeometryPtr> geometries;
};

inline void GLCompositeGeometry::SetVertexAttributes() const
{
  for(const GLGeometryPtr &g : geometries) g->SetVertexAttributes();
}

inline void GLCompositeGeometry::Buffer() const
{
  for(const GLGeometryPtr &g : geometries) g->Buffer();
}

inline void GLCompositeGeometry::Draw(const mat4 &model) const
{
  for(const GLGeometryPtr &g : geometries) g->Draw(model);
}
