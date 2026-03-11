#pragma once

#include <vector>

#include "GLBufferedGeometry.h"

enum class GLMazeType : unsigned char {
  Backtracking,
  Prim,
  Kruskal,
};

struct GLMazeCell {
  bool right : 1;
  bool up    : 1;
  bool left  : 1;
  bool down  : 1;
};

class GLMaze : public GLBufferedGeometryD<2> {
public:
  GLMaze(GLint width, GLint height, GLMazeType type = GLMazeType::Backtracking);
  ~GLMaze() override;

  GLint GetWidth() const { return width; }
  GLint GetHeight() const { return height; }
  GLint GetEntry() const { return entry; }
  GLint GetExit() const { return exit; }
  void GetEntry(GLint &x, GLint &y) const;
  void GetExit(GLint &x, GLint &y) const;
  const std::vector<std::vector<GLMazeCell>> &GetCells() const { return cells; }

  void GetNormalizedPosition(GLfloat x, GLfloat y, GLfloat &nx, GLfloat &ny) const;

protected:
  GLint width, height;
  GLint entry, exit;
  std::vector<std::vector<GLMazeCell>> cells;

  void GenerateVertices();
  void GenerateVertex(GLint x, GLint y);

  void IssueDraw() const override;
};
