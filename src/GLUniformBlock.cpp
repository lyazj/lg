#include "GLUniformBlock.h"

using namespace std;

GLUniformBlock::GLUniformBlock(GLuint b) : bindingPoint(b), buffer(GL_UNIFORM_BUFFER) { }

GLUniformBlock::~GLUniformBlock() { }

void GLUniformBlock::Bind() const { glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer.GetID()); }

void GLUniformBlock::Unbind() const { glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0); }
