#include "GLRenderable.h"

using namespace std;

GLRenderable::GLRenderable() : vertexAttributesSet(false), buffered(false)
{
  // empty
}

GLRenderable::~GLRenderable()
{
  // empty
}

void GLRenderable::SetVertexAttributes(bool force)
{
  if(!force && vertexAttributesSet) return;
  vertexAttributesSet = true;
  IssueSetVertexAttributes(force);
}

void GLRenderable::Buffer(bool force)
{
  if(!force && buffered) return;
  buffered = true;
  IssueBuffer(force);
}

void GLRenderable::Draw(const mat4 &model) const
{
  if(!vertexAttributesSet || !buffered) abort();
  IssueDraw(model);
}
