#include "GLTextureDecorator.h"

#include <math.h>

#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "GLProgram.h"
#include "GLTexture.h"
#include "GLVertexArray.h"

using namespace std;

void GLTextureDecorator::IssueDraw(const mat4 &model) const
{
  glActiveTexture(GL_TEXTURE0);
  if(texture) texture->Bind();
  GLSimpleRenderableDecorator::IssueDraw(model);
}

void GLBufferedTextureDecorator::IssueBuffer(bool force) const
{
  GLTextureDecorator::IssueBuffer(force);
  SetTexCoords();
  texCoordBuffer.Buffer(texCoords);
}

void GLBufferedTextureDecorator::IssueDraw(const mat4 &model) const
{
  GetVertexArray().Bind();
  texCoordBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_texCoord0", 2);
  GLTextureDecorator::IssueDraw(model);
  GLProgram::DisableVertexAttribute("a_texCoord0");
}

void GLPlanarTextureDecorator::SetTexCoords() const
{
  GLfloat xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
  GLint n = GetNVertex();
  for(GLint i = 0; i < n; ++i) {
    vec3 v;
    GetVertex(i, v);
    xmin = min(xmin, v.x);
    xmax = max(xmax, v.x);
    ymin = min(ymin, v.y);
    ymax = max(ymax, v.y);
  }

  texCoords.clear();
  texCoords.reserve(n);
  for(GLint i = 0; i < n; ++i) {
    vec3 v;
    GetVertex(i, v);
    GLfloat x = xmin == xmax ? 0.5f : (v.x - xmin) / (xmax - xmin);
    GLfloat y = ymin == ymax ? 0.5f : (v.y - ymin) / (ymax - ymin);
    texCoords.emplace_back(x, y);
  }
}

void GLSphericalTextureDecorator::SetTexCoords() const
{
  GLint n = GetNVertex();
  texCoords.clear();
  texCoords.reserve(n);
  for(GLint i = 0; i < n; ++i) {
    vec3 v;
    GetVertex(i, v);
    GLfloat theta = atan2f(hypotf(v.x, v.y), v.z);
    GLfloat phi = atan2f(v.y, v.x);
    if(phi < 0) phi += 2.0f * pi;
    texCoords.emplace_back(clamp(phi / (2.0f * pi), 0.0f, 1.0f), clamp(1.0f - theta / pi, 0.0f, 1.0f));
  }
}
