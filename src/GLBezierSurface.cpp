#include "GLBezierSurface.h"

#include <string.h>

#include <algorithm>
#include <glm/geometric.hpp>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

namespace {

class GLBezierSurfaceBuilder {
public:
  GLBezierSurfaceBuilder(const vec3 *c, vector<vec3> &v, vector<vec3> &no, vector<GLuint> &e)
      : controls(c), vertices(v), normals(no), elements(e)
  {
    // empty
  }
  void Build(GLint division);

private:
  const vec3 *controls;
  vector<vec3> &vertices;
  vector<vec3> &normals;
  vector<GLuint> &elements;
  GLuint n;

  void Build(GLuint istack, GLuint islice, GLint division);
  void BuildNormal(GLuint istack, GLuint islice);
  void HDivide(GLuint istack, GLuint islice, GLint division);
  void VDivide(GLuint istack, GLuint islice, GLint division);
};

void GLBezierSurfaceBuilder::Build(GLint division)
{
  n = 3 << division;
  vertices.resize((n + 1) * (n + 1));
  normals.resize((n + 1) * (n + 1));

  for(GLuint istack = 0; istack <= 3; ++istack) {
    for(GLuint islice = 0; islice <= 3; ++islice) {
      vertices[(istack << division) * (n + 1) + (islice << division)] = controls[istack * 4 + islice];
    }
  }

  Build(0, 0, division);
  for(GLuint istack = 0; istack <= n; istack += 3) {
    for(GLuint islice = 0; islice <= n; islice += 3) {
      normals[istack * (n + 1) + islice] = normalize(normals[istack * (n + 1) + islice]);
    }
  }

  elements.clear();
  elements.reserve(2 * (n / 3) * (n / 3 + 1));
  for(GLuint istack = 0; istack < n; istack += 3) {
    for(GLuint islice = 0; islice <= n; islice += 3) {
      elements.push_back(istack * (n + 1) + islice);
      elements.push_back((istack + 3) * (n + 1) + islice);
    }
  }
}

void GLBezierSurfaceBuilder::Build(GLuint istack, GLuint islice, GLint division)
{
  if(division == 0) return BuildNormal(istack, islice);
  for(GLuint i = 0; i <= 3; ++i) HDivide(istack + (i << division), islice, division);
  for(GLuint j = 0; j <= 6; ++j) VDivide(istack, islice + (j << (division - 1)), division);
  --division;
  for(GLuint i = 0; i < 2; ++i) {
    for(GLuint j = 0; j < 2; ++j) Build(istack + i * (3 << division), islice + j * (3 << division), division);
  }
}

void GLBezierSurfaceBuilder::BuildNormal(GLuint istack, GLuint islice)
{
  vec3 p00 = vertices[istack * (n + 1) + islice];
  vec3 p01 = vertices[istack * (n + 1) + islice + 1];
  vec3 p02 = vertices[istack * (n + 1) + islice + 2];
  vec3 p03 = vertices[istack * (n + 1) + islice + 3];
  vec3 p10 = vertices[(istack + 1) * (n + 1) + islice];
  vec3 p13 = vertices[(istack + 1) * (n + 1) + islice + 3];
  vec3 p20 = vertices[(istack + 2) * (n + 1) + islice];
  vec3 p23 = vertices[(istack + 2) * (n + 1) + islice + 3];
  vec3 p30 = vertices[(istack + 3) * (n + 1) + islice];
  vec3 p31 = vertices[(istack + 3) * (n + 1) + islice + 1];
  vec3 p32 = vertices[(istack + 3) * (n + 1) + islice + 2];
  vec3 p33 = vertices[(istack + 3) * (n + 1) + islice + 3];
  vec3 du00 = p10 - p00, dv00 = p01 - p00;
  vec3 du03 = p13 - p03, dv03 = p03 - p02;
  vec3 du30 = p30 - p20, dv30 = p31 - p30;
  vec3 du33 = p33 - p23, dv33 = p33 - p32;
  vec3 n00 = cross(du00, dv00);
  vec3 n03 = cross(du03, dv03);
  vec3 n30 = cross(du30, dv30);
  vec3 n33 = cross(du33, dv33);
  normals[istack * (n + 1) + islice] += normalize(n00);
  normals[istack * (n + 1) + islice + 3] += normalize(n03);
  normals[(istack + 3) * (n + 1) + islice] += normalize(n30);
  normals[(istack + 3) * (n + 1) + islice + 3] += normalize(n33);
}

void GLBezierSurfaceBuilder::HDivide(GLuint istack, GLuint islice, GLint division)
{
  vec3 p0 = vertices[istack * (n + 1) + islice];
  vec3 p1 = vertices[istack * (n + 1) + islice + (1 << division)];
  vec3 p2 = vertices[istack * (n + 1) + islice + (2 << division)];
  vec3 p3 = vertices[istack * (n + 1) + islice + (3 << division)];
  vec3 l1 = (p0 + p1) * 0.5f;
  vec3 r2 = (p2 + p3) * 0.5f;
  vec3 p12 = (p1 + p2) * 0.5f;
  vec3 l2 = (l1 + p12) * 0.5f;
  vec3 r1 = (r2 + p12) * 0.5f;
  vec3 l3 = (l2 + r1) * 0.5f;
  --division;
  vertices[istack * (n + 1) + islice + (1 << division)] = l1;
  vertices[istack * (n + 1) + islice + (2 << division)] = l2;
  vertices[istack * (n + 1) + islice + (3 << division)] = l3;
  vertices[istack * (n + 1) + islice + (4 << division)] = r1;
  vertices[istack * (n + 1) + islice + (5 << division)] = r2;
}

void GLBezierSurfaceBuilder::VDivide(GLuint istack, GLuint islice, GLint division)
{
  vec3 p0 = vertices[istack * (n + 1) + islice];
  vec3 p1 = vertices[(istack + (1 << division)) * (n + 1) + islice];
  vec3 p2 = vertices[(istack + (2 << division)) * (n + 1) + islice];
  vec3 p3 = vertices[(istack + (3 << division)) * (n + 1) + islice];
  vec3 l1 = (p0 + p1) * 0.5f;
  vec3 r2 = (p2 + p3) * 0.5f;
  vec3 p12 = (p1 + p2) * 0.5f;
  vec3 l2 = (l1 + p12) * 0.5f;
  vec3 r1 = (r2 + p12) * 0.5f;
  vec3 l3 = (l2 + r1) * 0.5f;
  --division;
  vertices[(istack + (1 << division)) * (n + 1) + islice] = l1;
  vertices[(istack + (2 << division)) * (n + 1) + islice] = l2;
  vertices[(istack + (3 << division)) * (n + 1) + islice] = l3;
  vertices[(istack + (4 << division)) * (n + 1) + islice] = r1;
  vertices[(istack + (5 << division)) * (n + 1) + islice] = r2;
}

}  // namespace

GLBezierSurface::GLBezierSurface(vec3 c[16], GLint d) : elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  memcpy(controls, c, sizeof controls);
  division = clamp<GLint>(d, 0, 14);

  GLBezierSurfaceBuilder(controls, vertices, normals, elements).Build(division);
}

GLBezierSurface::~GLBezierSurface()
{
  // empty
}

void GLBezierSurface::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  normalBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_normal", 3);
}

void GLBezierSurface::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  normalBuffer.Buffer(normals);
  elementBuffer.Buffer(elements);
}

void GLBezierSurface::IssueDraw() const
{
  GLuint n = 1 << division;
  GLuint *offset = nullptr;
  for(GLuint istack = 0; istack < n; ++istack) {
    GLDrawElements(GL_TRIANGLE_STRIP, 2 * (n + 1), GL_UNSIGNED_INT, offset);
    offset += 2 * (n + 1);
  }
}
