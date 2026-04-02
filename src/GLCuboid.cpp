#include "GLCuboid.h"

#include <glm/gtc/matrix_transform.hpp>

#include "GLRectangle.h"
#include "GLTransformedRenderable.h"

using namespace std;

GLCuboid::GLCuboid(GLfloat l, GLfloat w, GLfloat h) : length(l), width(w), height(h)
{
  mat4 transform;
  Reserve(6);

  // Up and down.
  auto ud = make_shared<GLRectangle>(length, width);
  transform = translate(mat4(1.0f), vec3(0.0f, 0.0f, height / 2.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(ud, transform));  // up
  transform = translate(mat4(1.0f), vec3(0.0f, 0.0f, -height / 2.0f));
  transform = rotate(transform, 180.0f * deg, vec3(1.0f, 0.0f, 0.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(ud, transform));  // down

  // Left and right.
  auto lr = make_shared<GLRectangle>(width, height);
  transform = translate(mat4(1.0f), vec3(length / 2.0f, 0.0f, 0.0f));
  transform = rotate(transform, 90.0f * deg, vec3(0.0f, 1.0f, 0.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(lr, transform));  // right
  transform = translate(mat4(1.0f), vec3(-length / 2.0f, 0.0f, 0.0f));
  transform = rotate(transform, -90.0f * deg, vec3(0.0f, 1.0f, 0.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(lr, transform));  // left

  // Front and back.
  auto fb = make_shared<GLRectangle>(length, height);
  transform = translate(mat4(1.0f), vec3(0.0f, -width / 2.0f, 0.0f));
  transform = rotate(transform, 90.0f * deg, vec3(1.0f, 0.0f, 0.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(fb, transform));  // front
  transform = translate(mat4(1.0f), vec3(0.0f, width / 2.0f, 0.0f));
  transform = rotate(transform, -90.0f * deg, vec3(1.0f, 0.0f, 0.0f));
  AddRenderable(make_shared<GLTransformedRenderable>(fb, transform));  // back
}

GLCuboid::~GLCuboid()
{
  // empty
}

GLSimpleRenderablePtr GLCuboid::GetFace(GLint i) const
{
  auto face = (GLTransformedRenderable *)GetRenderable(i).get();
  return static_pointer_cast<GLSimpleRenderable>(face->GetRenderable());
}

void GLCuboid::SetFace(GLint i, const GLSimpleRenderablePtr &r)
{
  auto face = (GLTransformedRenderable *)GetRenderable(i).get();
  face->SetRenderable(r);
}

void GLCuboid::GetVertex(GLint i, GLint j, vec3 &v) const
{
  GetFace(i)->GetVertex(j, v);
  auto face = (GLTransformedRenderable *)GetRenderable(i).get();
  v = face->GetModel() * vec4(v, 1.0f);
}
