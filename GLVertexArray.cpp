#include "GLVertexArray.h"

using namespace std;

GLVertexArray::GLVertexArray()
{
  glGenVertexArrays(1, &id);
  if(id == 0) abort();
}

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &id); }

void GLVertexArray::Bind() const { glBindVertexArray(id); }

void GLVertexArray::Unbind() const { glBindVertexArray(0); }
