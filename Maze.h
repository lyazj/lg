#pragma once

#include <glm/vec2.hpp>
#include <vector>

#include "GLBasicGeometry.h"

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

class Maze : public GLBasicGeometry {
public:
  Maze(GLint width, GLint height, MazeType type = MazeType::Backtracking);
  ~Maze() override;

  GLint GetWidth() const { return width; }
  GLint GetHeight() const { return height; }
  GLint GetEntry() const { return entry; }
  GLint GetExit() const { return exit; }
  const std::vector<std::vector<MazeCell>> &GetCells() const { return cells; }

protected:
  GLint width, height;
  GLint entry, exit;
  std::vector<std::vector<MazeCell>> cells;
  std::vector<glm::vec2> vertices;

  void GenerateVertices();
  void GenerateVertex(GLint x, GLint y);

  void IssueDraw() const override;
  void IssueBuffer() const override;
};
