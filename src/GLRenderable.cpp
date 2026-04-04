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
