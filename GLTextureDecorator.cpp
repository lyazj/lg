#include "GLTextureDecorator.h"

#include <math.h>

#include <algorithm>
#include <glm/vec3.hpp>

#include "GLProgram.h"
#include "GLTexture.h"
#include "GLVertexArray.h"

using namespace std;

void GLTextureDecorator::IssueDraw(const mat4 &model) const
{
  glActiveTexture(GL_TEXTURE0);
  texture->Bind();
  GLSimpleRenderableDecorator::IssueDraw(model);
}

void GLBufferedTextureDecorator::Buffer() const
{
  GLTextureDecorator::Buffer();
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

void GL2DTextureDecorator::SetTexCoords() const
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
