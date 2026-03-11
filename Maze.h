#pragma once

#include <vector>

#include "GLElementGeometry.h"

enum class MazeType : unsigned char {
  Backtracking,
  Prim,
  Kruskal,
};

struct MazeCell {
  bool right : 1;
  bool up    : 1;
  bool left  : 1;
  bool down  : 1;
};

class Maze : public GLElementGeometryD<2> {
public:
  Maze(GLint width, GLint height, MazeType type = MazeType::Backtracking);
  ~Maze() override;

  GLint GetWidth() const { return width; }
  GLint GetHeight() const { return height; }
  GLint GetEntry() const { return entry; }
  GLint GetExit() const { return exit; }
  void GetEntry(GLint &x, GLint &y) const;
  void GetExit(GLint &x, GLint &y) const;
  const std::vector<std::vector<MazeCell>> &GetCells() const { return cells; }

  void GetNormalizedPosition(GLfloat x, GLfloat y, GLfloat &nx, GLfloat &ny) const;

protected:
  GLint width, height;
  GLint entry, exit;
  std::vector<std::vector<MazeCell>> cells;

  void GenerateVertices();
  void GenerateVertex(GLint x, GLint y);

  void IssueDraw() const override;
};
