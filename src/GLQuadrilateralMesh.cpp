#include "GLQuadrilateralMesh.h"

#include <algorithm>
#include <glm/geometric.hpp>

#include "GLProgram.h"
#include "Utils.h"

using namespace std;

GLQuadrilateralMesh::GLQuadrilateralMesh(GLint st, GLint sl)
    : stacks(max<GLint>(1, st)), slices(max<GLint>(1, sl)), elementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  Resize(stacks, slices);
}

GLQuadrilateralMesh::~GLQuadrilateralMesh()
{
  // empty
}

void GLQuadrilateralMesh::Resize(GLint st, GLint sl)
{
  if((unsigned long long)(st + 1) * (sl + 1) >= ((unsigned long long)1 << 30)) abort();
  stacks = st, slices = sl;

  // Reallocate CPU-side storage.
  vertices.resize((stacks + 1) * (slices + 1));
  normals.resize((stacks + 1) * (slices + 1));
  texCoords.resize((stacks + 1) * (slices + 1));
  elements.resize(2 * stacks * (slices + 1));

  // Texture coordinates depend only on grid topology so they can be computed once here.
  for(GLint istack = 0; istack <= stacks; ++istack) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      texCoords[istack * (slices + 1) + islice] = {
        (GLfloat)islice / (GLfloat)slices,
        1.0f - (GLfloat)istack / (GLfloat)stacks,
      };
    }
  }

  // Element indices depend only on grid topology so they can also be computed once here.
  for(GLint istack = 0; istack < stacks; ++istack) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      elements[2 * (istack * (slices + 1) + islice) + 0] = istack * (slices + 1) + islice;
      elements[2 * (istack * (slices + 1) + islice) + 1] = (istack + 1) * (slices + 1) + islice;
    }
  }

  // Upload updated buffers to GPU once here.
  vertexArray.Bind();
  texCoordBuffer.Buffer(texCoords);
  elementBuffer.Buffer(elements);
}

void GLQuadrilateralMesh::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  normalBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_normal", 3);
  texCoordBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_texCoord0", 2);
}

void GLQuadrilateralMesh::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();

  // Compute vertex normals.
  for(GLint istack = 0; istack <= stacks; ++istack) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      vec3 tstack, tslice;
      if(istack == 0) {
        tstack = ((*this)[istack + 1][islice] - (*this)[istack][islice]) * 2.0f;
      } else if(istack == stacks) {
        tstack = ((*this)[istack][islice] - (*this)[istack - 1][islice]) * 2.0f;
      } else {
        tstack = (*this)[istack + 1][islice] - (*this)[istack - 1][islice];
      }
      if(islice == 0) {
        tslice = ((*this)[istack][islice + 1] - (*this)[istack][islice]) * 2.0f;
      } else if(islice == slices) {
        tslice = ((*this)[istack][islice] - (*this)[istack][islice - 1]) * 2.0f;
      } else {
        tslice = (*this)[istack][islice + 1] - (*this)[istack][islice - 1];
      }
      normals[istack * (slices + 1) + islice] = normalize(cross(tstack, tslice));
    }
  }

  normalBuffer.SetUsage(vertexBuffer.GetUsage());
  normalBuffer.Buffer(normals);
}

void GLQuadrilateralMesh::IssueDraw() const
{
  GLuint *offset = nullptr;
  for(GLint istack = 0; istack < stacks; ++istack) {
    GLDrawElements(GL_TRIANGLE_STRIP, 2 * (slices + 1), GL_UNSIGNED_INT, offset);
    offset += 2 * (slices + 1);
  }
}
