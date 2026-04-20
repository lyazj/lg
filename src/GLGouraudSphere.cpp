#include "GLGouraudSphere.h"

#include <math.h>

#include <algorithm>
#include <glm/geometric.hpp>
#include <unordered_map>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLGouraudSphere::GLGouraudSphere(GLfloat r, GLint o)
    : radius(r), order(clamp<GLint>(o, 0, 15)), elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  vertices.reserve((size_t)pow(3, order + 1) + 1);
  elements.reserve((size_t)3 << ((order + 1) << 1));

  unordered_map<uint64_t, GLuint> vertexMap;  // vertex barycentric -> vertex index
  vector<vector<GLuint>> vertexFacials;       // vertex index -> facial indices
  vector<vec3> facialNormals;                 // facial index -> facial normal

  vertexFacials.reserve(vertices.capacity());
  facialNormals.reserve(elements.capacity() / 3);

  // Initialize with a regular tetrahedron inscribed in the sphere.
  GLfloat cos_thetas[4] = { 1.0f, -1.0f / 3.0f, -1.0f / 3.0f, -1.0f / 3.0f };
  GLfloat sin_thetas[4] = { 0.0f, sqrtf(8.0f) / 3.0f, sqrtf(8.0f) / 3.0f, sqrtf(8.0f) / 3.0f };
  GLfloat phis[4] = { 0.0f, 0.0f, 2.0f * pi / 3.0f, 4.0f * pi / 3.0f };
  for(GLint i = 0; i < 4; ++i) {
    GLfloat z = radius * cos_thetas[i], rho = radius * sin_thetas[i];
    GLfloat x = rho * cosf(phis[i]), y = rho * sinf(phis[i]);
    vertices.emplace_back(x, y, z);
    vertexFacials.emplace_back();
    vertexMap.emplace((uint64_t)1 << order << (i << 4), i);
  }

  // Unique the vertices by the barycentric coordinates and map them to the vertex indices.
  auto getMidPoint = [this, &vertexMap, &vertexFacials](uint64_t a, uint64_t b) -> uint64_t {
    uint64_t m = (a + b) >> 1;
    auto it = vertexMap.find(m);
    if(it == vertexMap.end()) {  // first reached
      GLuint va = vertexMap.at(a), vb = vertexMap.at(b), vc = (GLuint)vertices.size();
      vertices.push_back(normalize(vertices[va] + vertices[vb]) * radius);
      vertexFacials.emplace_back();
      it = vertexMap.emplace(m, vc).first;
    }
    return m;
  };

  // Recursively divide one spherical triangle.
  auto divide = [this, &getMidPoint, &vertexMap, &vertexFacials, &facialNormals](
                    const auto &self, uint64_t a, uint64_t b, uint64_t c, int ord) {
    if(ord-- == 0) {
      GLuint va = vertexMap.at(a), vb = vertexMap.at(b), vc = vertexMap.at(c);
      elements.push_back(va);
      elements.push_back(vb);
      elements.push_back(vc);
      vertexFacials[va].push_back((GLuint)facialNormals.size());
      vertexFacials[vb].push_back((GLuint)facialNormals.size());
      vertexFacials[vc].push_back((GLuint)facialNormals.size());
      vec3 ab = vertices[vb] - vertices[va], ac = vertices[vc] - vertices[va];
      facialNormals.push_back(normalize(cross(ab, ac)));
      return;
    }
    uint64_t d = getMidPoint(a, b), e = getMidPoint(b, c), f = getMidPoint(c, a);
    self(self, a, d, f, ord);
    self(self, b, e, d, ord);
    self(self, c, f, e, ord);
    self(self, d, e, f, ord);
  };

  divide(divide, (uint64_t)1 << order << (0 << 4), (uint64_t)1 << order << (1 << 4), (uint64_t)1 << order << (2 << 4),
      order);
  divide(divide, (uint64_t)1 << order << (0 << 4), (uint64_t)1 << order << (2 << 4), (uint64_t)1 << order << (3 << 4),
      order);
  divide(divide, (uint64_t)1 << order << (0 << 4), (uint64_t)1 << order << (3 << 4), (uint64_t)1 << order << (1 << 4),
      order);
  divide(divide, (uint64_t)1 << order << (3 << 4), (uint64_t)1 << order << (2 << 4), (uint64_t)1 << order << (1 << 4),
      order);

  // Compute the normal vector for each vertex combining its adjacent facials.
  vertexMap = {};  // Save some space.
  normals.reserve(vertices.size());
  for(size_t i = 0; i < vertices.size(); ++i) {
    vec3 n(0.0f);
    for(GLuint f : vertexFacials[i]) n += facialNormals[f];
    normals.push_back(normalize(n));
  }
}

GLGouraudSphere::~GLGouraudSphere()
{
  // empty
}

void GLGouraudSphere::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  normalBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_normal", 3);
}

void GLGouraudSphere::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  normalBuffer.Buffer(normals);
  elementBuffer.Buffer(elements);
}

void GLGouraudSphere::IssueDraw() const
{
  GLDrawElements(GL_TRIANGLES, (GLsizei)elements.size(), GL_UNSIGNED_INT, nullptr);
}
