#include "GLTextureDecorator.h"

#include <algorithm>

#include "GLApplication.h"
#include "GLTexture.h"

using namespace std;

void GLTextureDecorator::Draw(const mat4 &model) const
{
  glActiveTexture(GL_TEXTURE0);
  texture->Bind();

  if(program) {
    GLApplication *app = GLApplication::GetInstance();
    GLProgramPtr previousProgram = app->GetProgram();
    app->UseProgram(program);
    renderable->Draw(model);
    app->UseProgram(previousProgram);
  } else {
    renderable->Draw(model);
  }
}

void GLBufferedTextureDecorator::Buffer() const
{
  renderable->Buffer();
  SetTexCoords();
  texCoordBuffer.Buffer(texCoords);
}

void GL2DTextureDecorator::SetTexCoords() const
{
  GLfloat xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
  GLint n = renderable->GetNVertex();
  for(GLint i = 0; i < n; ++i) {
    vec3 v;
    renderable->GetVertex(i, v);
    xmin = min(xmin, v.x);
    xmax = max(xmax, v.x);
    ymin = min(ymin, v.y);
    ymax = max(ymax, v.y);
  }

  texCoords.reserve(n);
  for(GLint i = 0; i < n; ++i) {
    vec3 v;
    renderable->GetVertex(i, v);
    texCoords.emplace_back((v.x - xmin) / (xmax - xmin), (v.y - ymin) / (ymax - ymin));
  }
}
